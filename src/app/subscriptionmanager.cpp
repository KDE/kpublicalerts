/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "subscriptionmanager.h"
#include "geomath.h"
#include "restapi.h"

#include <KLocalizedString>

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkInformation>
#include <QNetworkReply>
#include <QSettings>
#include <QTimer>

#include <cmath>

using namespace Qt::Literals;
using namespace KPublicAlerts;

SubscriptionManager::SubscriptionManager(QObject *parent)
    : QAbstractListModel(parent)
    , m_connector(QStringLiteral("org.kde.publicalerts"))
{
    connect(&m_connector, &KUnifiedPush::Connector::endpointChanged, this, &SubscriptionManager::doSubscribeAll);
    connect(&m_connector, &KUnifiedPush::Connector::messageReceived, this, [this](const QByteArray &msg) {
        qDebug() << msg;
        const auto msgObj = QJsonDocument::fromJson(msg).object();
        const auto type = msgObj.value("type"_L1).toString();
        if (type == "added"_L1) {
            if (const auto id = msgObj.value("alert_id"_L1).toString(); !id.isEmpty()) {
                Q_EMIT alertAdded(id);
                return;
            }
        }
        if (type == "update"_L1) {
            if (const auto id = msgObj.value("alert_id"_L1).toString(); !id.isEmpty()) {
                Q_EMIT alertUpdated(id);
                return;
            }
        }
        if (type == "removed"_L1) {
            if (const auto id = msgObj.value("alert_id"_L1).toString(); !id.isEmpty()) {
                Q_EMIT alertRemoved(id);
                return;
            }
        }
        if (type == "subscribe"_L1) {
            const auto confirmationId = msgObj.value("confirmation_id"_L1).toString();
            const auto it = std::find_if(m_subscriptions.begin(), m_subscriptions.end(), [confirmationId](const auto &sub) {
                return sub.m_pendingConfirmation == confirmationId;
            });
            if (confirmationId.isEmpty()) {
                qWarning() << "incomplete subscription push notification:" << msgObj;
                return;
            }
            if (it != m_subscriptions.end()) {
                (*it).m_pendingConfirmation.clear();
                QSettings settings;
                (*it).store(settings);
                const int row = (int)std::distance(m_subscriptions.begin(), it);
                Q_EMIT dataChanged(index(row, 0), index(row, 0));
                qDebug() << "got confirmation for subscription" << (*it).m_name;
            } else {
                m_confirmations.insert(confirmationId);
                qDebug() << "got confirmation for not yet completed subscription" << confirmationId;
            }
        }
        // TODO handle "unsubscribe"

        // the server tries to tell us something but we don't know what,
        // so try to update everything
        Q_EMIT unhandledPushNotifications();
    });

    QSettings settings;
    const auto subIds = settings.value(QLatin1String("SubscriptionIds"), QStringList()).toStringList();
    m_subscriptions.reserve(subIds.size());
    for (const auto &subId : subIds) {
        m_subscriptions.push_back(Subscription::load(subId, settings));
    }
    std::sort(m_subscriptions.begin(), m_subscriptions.end());

    m_connector.setVapidPublicKeyRequired(true);
    m_connector.registerClient(i18n("Weather and emergency alert notifications")); // TODO technically we only needs this when there is at least one subscription

    connect(this, &QAbstractItemModel::rowsInserted, this, &SubscriptionManager::countChanged);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &SubscriptionManager::countChanged);

    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setTimerType(Qt::VeryCoarseTimer);
    m_heartbeatTimer->setInterval(std::chrono::hours(4));
    m_heartbeatTimer->setSingleShot(false);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &SubscriptionManager::checkHeartbeat);

    QNetworkInformation::loadBackendByFeatures(QNetworkInformation::Feature::Reachability);
    if (auto ni = QNetworkInformation::instance(); ni) {
        connect(ni, &QNetworkInformation::reachabilityChanged, this, [this](auto reachability) {
            if (reachability == QNetworkInformation::Reachability::Online) {
                checkHeartbeat();
            }
        });
    } else {
        qWarning() << "No network status monitoring available!";
    }
}

SubscriptionManager::~SubscriptionManager() = default;

void SubscriptionManager::setNetworkAccessManager(QNetworkAccessManager *nam)
{
    m_nam = nam;
    fetchVapidKey();
    doSubscribeAll();
    checkHeartbeat();
    m_heartbeatTimer->start();
}

int SubscriptionManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_subscriptions.size();
}

QVariant SubscriptionManager::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index)) {
        return {};
    }

    const auto &sub = m_subscriptions[index.row()];
    switch (role) {
        case Qt::DisplayRole:
            return sub.m_name;
        case SubscriptionRole:
            return QVariant::fromValue(sub);
        case IsSubscribedRole:
            return sub.isSubscribed();
        case AwaitsConfirmationRole:
            return sub.awaitsConfirmation();
    }

    return {};
}

QHash<int, QByteArray> SubscriptionManager::roleNames() const
{
    auto n = QAbstractListModel::roleNames();
    n.insert(SubscriptionRole, "subscription");
    n.insert(IsSubscribedRole, "isSubscribed");
    n.insert(AwaitsConfirmationRole, "awaitsConfirmation");
    return n;
}

bool SubscriptionManager::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid()) {
        return false;
    }

    for (int i = row; i < row + count; ++i) {
        const auto id = m_subscriptions[i].m_id;
        if (m_subscriptions[i].m_subscriptionId.isNull()) { // not subscribed, so we can just remove this
            doRemoveOne(id);
            continue;
        }

        auto reply = m_nam->deleteResource(RestApi::unsubscribe(m_subscriptions[i].m_subscriptionId));
        connect(reply, &QNetworkReply::finished, this, [this, reply, id]() {
            reply->deleteLater();
            if (reply->error() != QNetworkReply::NoError && reply->error() != QNetworkReply::ContentNotFoundError) {
                qWarning() << reply->errorString();
                // TODO
            } else {
                doRemoveOne(id);
            }
        });
    }

    return false;
}

void SubscriptionManager::doRemoveOne(const QString& id)
{
    const auto it = std::lower_bound(m_subscriptions.begin(), m_subscriptions.end(), id);
    if (it == m_subscriptions.end() || (*it).m_id != id) {
        return;
    }
    const auto row = std::distance(m_subscriptions.begin(), it);
    beginRemoveRows({}, row, row);
    m_subscriptions.erase(it);
    endRemoveRows();

    QSettings settings;
    storeSubscriptionIds(settings);
}

void SubscriptionManager::addSubscription(float lat, float lon, float radius, const QString& name)
{
    Subscription s;
    s.m_id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    s.m_boundingBox = GeoMath::boundingBoxForCircle(lat, lon, radius);
    s.m_name = name;

    const auto it = std::lower_bound(m_subscriptions.begin(), m_subscriptions.end(), s);
    beginInsertRows({}, std::distance(m_subscriptions.begin(), it), std::distance(m_subscriptions.begin(), it));
    m_subscriptions.insert(it, std::move(s));
    endInsertRows();

    QSettings settings;
    s.store(settings);
    storeSubscriptionIds(settings);

    doSubscribeAll();
}

const std::vector<Subscription> &SubscriptionManager::subscriptions() const
{
    return m_subscriptions;
}

void SubscriptionManager::doSubscribeAll()
{
    const auto upEndpoint = m_connector.endpoint();
    for (const auto &s : m_subscriptions) {
        if (!upEndpoint.isEmpty()) { // push notifications available
            if (s.isSubscribed() && s.m_notificationEndpoint != upEndpoint) { // push notification endpoint changed
                doUnsubscribeOne(s);
            }
            if (!s.isSubscribed() || s.m_notificationEndpoint != upEndpoint) {
                doSubscribeOne(s);
            }
        } else { // push notifications not available
            if (s.isSubscribed()) {
                doUnsubscribeOne(s);
            }
        }
    }
}

void SubscriptionManager::doSubscribeOne(const Subscription &sub)
{
    // TODO sub needs a "(un)subscribing" flag
    const auto id = sub.m_id;
    const auto upEndpoint = m_connector.endpoint();

    QJsonObject subCmd{
        {"push_service"_L1, "UNIFIED_PUSH_ENCRYPTED"_L1},
        {"token"_L1, upEndpoint},
        {"min_lon"_L1, sub.m_boundingBox.left()},
        {"max_lon"_L1, sub.m_boundingBox.right()},
        {"min_lat"_L1, sub.m_boundingBox.top()},
        {"max_lat"_L1, sub.m_boundingBox.bottom()},
        {"p256dh_key"_L1, QString::fromLatin1(m_connector.contentEncryptionPublicKey().toBase64(QByteArray::Base64UrlEncoding))},
        {"auth_key"_L1, QString::fromLatin1(m_connector.contentEncryptionAuthSecret().toBase64(QByteArray::Base64UrlEncoding))},
    };

    auto reply = m_nam->post(RestApi::subscribe(), QJsonDocument(subCmd).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [this, reply, id, upEndpoint]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << reply->errorString() << reply->readAll(); // TODO
            return;
        }

        const auto it = std::lower_bound(m_subscriptions.begin(), m_subscriptions.end(), id);
        if (it == m_subscriptions.end() || (*it).m_id != id) {
            return;
        }

        const auto subRes = QJsonDocument::fromJson(reply->readAll()).object();
        qDebug() << subRes;
        (*it).m_subscriptionId = QUuid(subRes.value("subscription_id"_L1).toString());
        (*it).m_pendingConfirmation = subRes.value("confirmation_id"_L1).toString();
        // got the push notification before the REST response
        if (auto cit = m_confirmations.find((*it).m_pendingConfirmation); cit != m_confirmations.end()) {
            m_confirmations.erase(cit);
            (*it).m_pendingConfirmation.clear();
            qDebug() << "subcription has already been confirmed";
        }
        (*it).m_notificationEndpoint = upEndpoint;
        (*it).m_lastHeartbeat = QDateTime::currentDateTime();

        QSettings settings;
        (*it).store(settings);

        const auto row = (int)std::distance(m_subscriptions.begin(), it);
        Q_EMIT dataChanged(index(row, 0), index(row, 0));
    });
}

void SubscriptionManager::doUnsubscribeOne(const Subscription &sub)
{
    auto reply = m_nam->deleteResource(RestApi::unsubscribe(sub.m_subscriptionId));
    connect(reply, &QNetworkReply::finished, this, [reply, sub, this]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError && reply->error() != QNetworkReply::ContentNotFoundError) {
            qWarning() << reply->errorString();
        }
        const auto it = std::lower_bound(m_subscriptions.begin(), m_subscriptions.end(), sub.m_id);
        if (it != m_subscriptions.end() && (*it).m_id == sub.m_id && (*it).m_subscriptionId == sub.m_subscriptionId) {
            (*it).m_subscriptionId = {};

            QSettings settings;
            (*it).store(settings);

            const auto idx = index(std::distance(m_subscriptions.begin(), it), 0);
            Q_EMIT dataChanged(idx, idx);
        }
    });
}

void SubscriptionManager::storeSubscriptionIds(QSettings &settings)
{
    QStringList l;
    l.reserve(m_subscriptions.size());
    std::transform(m_subscriptions.begin(), m_subscriptions.end(), std::back_inserter(l), [](const auto &s) { return s.m_id; });
    settings.setValue(QLatin1String("SubscriptionIds"), l);
}

void SubscriptionManager::checkHeartbeat()
{
    const auto threshold = QDateTime::currentDateTime().addDays(-1); // TODO make this less aggressive
    for (const auto &s : m_subscriptions) {
        if (s.m_lastHeartbeat.isValid() && s.m_lastHeartbeat > threshold) {
            continue;
        }

        qDebug() << s.m_id << s.m_subscriptionId << "needs a heartbeat";
        const auto id = s.m_id;
        auto reply = m_nam->put(RestApi::heartbeat(s.m_subscriptionId), QByteArray());
        connect(reply, &QNetworkReply::finished, this, [reply, id, this]() {
            reply->deleteLater();
            const auto it = std::lower_bound(m_subscriptions.begin(), m_subscriptions.end(), id);
            if (it == m_subscriptions.end() || (*it).m_id != id) {
                return;
            }

            if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
                (*it).m_lastHeartbeat = QDateTime::currentDateTime();
                QSettings settings;
                (*it).store(settings);
                qDebug() << "subscription renewed" << id << (*it).m_lastHeartbeat;
                return;
            }

            if (reply->error() == QNetworkReply::ContentNotFoundError) {
                // subscription expired, resubscribe
                qDebug() << "resubscription needed for" << id;
                doSubscribeOne(*it);
                return;
            }

            // we retry on network errors via QNetworkInformation reachability monitoring
            qDebug() << reply->errorString() << reply->readAll() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        });
    }
}

void SubscriptionManager::fetchVapidKey()
{
    if (!m_connector.vapidPublicKey().isEmpty()) {
        return;
    }

    auto reply = m_nam->get(RestApi::vapidKey());
    connect(reply, &QNetworkReply::finished, this, [reply, this]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Failed to fetch VAPID key:" << reply->errorString();
            return;
        }

        const auto response = QJsonDocument::fromJson(reply->readAll()).object();
        m_connector.setVapidPublicKey(response.value("vapid-key"_L1).toString());
    });
}

#include "moc_subscriptionmanager.cpp"
