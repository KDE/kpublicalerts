/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "alertsmanager.h"
#include "caputil.h"
#include "geomath.h"
#include "restapi.h"
#include "subscriptionmanager.h"

#include <KWeatherCore/CAPParser>
#include <KWeatherCore/CAPArea>
#include <KWeatherCore/CAPReference>

#include <KLocalizedString>
#include <KNotification>

#include <QDir>
#include <QDirIterator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QUrlQuery>

using namespace KPublicAlerts;

bool AlertElement::operator<(const AlertElement &other) const
{
    return id < other.id;
}

bool AlertElement::operator<(const QString &otherId) const
{
    return id < otherId;
}

bool AlertElement::isValid() const
{
    // TODO should be in KWC instead?
    return !alertData.alertInfos().empty();
}

QDateTime AlertElement::expireTime() const
{
    QDateTime dt;
    for (const auto &info : alertData.alertInfos()) {
        if (info.expireTime().isValid()) {
            dt = dt.isValid() ? std::max(dt, info.expireTime()) : info.expireTime();
        }
    }
    return dt;
}

bool AlertElement::isExpired() const
{
    const auto dt = expireTime();
    return dt.isValid() && dt <= QDateTime::currentDateTime();
}

QDateTime AlertElement::onsetTime() const
{
    QDateTime dt;
    for (const auto &info : alertData.alertInfos()) {
        if (info.onsetTime().isValid()) {
            dt = dt.isValid() ? std::min(dt, info.onsetTime()) : info.onsetTime();
        }
    }
    return dt;
}

KWeatherCore::CAPAlertMessage AlertElement::alert() const
{
    return alertData;
}

KWeatherCore::CAPAlertInfo AlertElement::info() const
{
    return alertData.alertInfos()[alertData.preferredInfoIndexForLocale()];
}

static QString basePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1String("/cap/");
}

AlertsManager::AlertsManager(QObject* parent)
    : QAbstractListModel(parent)
{
    for (auto it = QDirIterator(basePath(), QDir::Files); it.hasNext();) {
        it.next();
        if (!it.fileName().endsWith(QLatin1String(".xml"))) {
            continue;
        }

        QFile f(it.filePath());
        if (!f.open(QFile::ReadOnly)) {
            qWarning() << f.fileName() << f.errorString();
            continue;
        }

        KWeatherCore::CAPParser p(f.readAll());
        AlertElement e;
        e.id = it.fileName().chopped(4);
        e.alertData = p.parse();
        if (!e.isValid() || e.isExpired()) {
            qDebug() << "dropping expired or invalid cache file:" << f.fileName();
            f.remove();
            continue;
        }

        addAlert(std::move(e));
    }

    m_expireTimer.setTimerType(Qt::VeryCoarseTimer);
    m_expireTimer.setSingleShot(true);
    connect(&m_expireTimer, &QTimer::timeout, this, &AlertsManager::purgeExpired);
    scheduleExpire();
}

AlertsManager::~AlertsManager() = default;

void AlertsManager::setNetworkAccessManager(QNetworkAccessManager *nam)
{
    m_nam = nam;
}

void AlertsManager::setSubscriptionManager(SubscriptionManager *subMgr)
{
    m_subMgr = subMgr;
}

void AlertsManager::fetchAlert(const QString &id, bool force)
{
    if (!force) {
        if (const auto it = std::lower_bound(m_alerts.begin(), m_alerts.end(), id); it != m_alerts.end() && (*it).id == id) {
            qDebug() << "alert" << id << "already known, not fetching";
            return;
        }
    }

    auto reply = m_nam->get(RestApi::alert(id));
    connect(reply, &QNetworkReply::finished, this, [this, id, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << reply->errorString();
            return;
        }
        const auto capData = reply->readAll();
        KWeatherCore::CAPParser p(capData);

        AlertElement e;
        e.id = id;
        e.alertData = p.parse();
        if (!e.isValid() || e.isExpired()) {
            qDebug() << "dropping expired or invalid alert:" << e.alertData.identifier();
            return;
        }

        // drop all alert message we are supposed to ignore as a client
        if (e.alertData.status() == KWeatherCore::CAPAlertMessage::Status::System ||
            e.alertData.status() == KWeatherCore::CAPAlertMessage::Status::Test ||
            e.alertData.status() == KWeatherCore::CAPAlertMessage::Status::Draft)
        {
            qDebug() << "ignoring alert message with internal status:" << e.alertData.identifier() << e.alertData.status();
            return;
        }

        // do precise hit detection rather than relying on any simplification the server works with
        const auto hit = intersectsSubscribedArea(e);
        if (!hit) {
            qDebug() << "dropping alert failing precise hit detection:" << e.alertData.identifier();
            return;
        }

        const auto path = basePath();
        QDir().mkpath(path);
        if (QFile f(path + id + QLatin1String(".xml")); f.open(QFile::WriteOnly)) {
            f.write(capData);
        } else {
            qWarning() << f.fileName() << f.errorString();
        }

        showNotification(addAlert(std::move(e)));
        scheduleExpire();
    });
}

void AlertsManager::fetchAll()
{
    for (const auto &subscription : m_subMgr->subscriptions()) {
        auto reply = m_nam->get(RestApi::alerts(subscription.m_boundingBox));
        ++m_pendingFetchJobs;
        if (m_pendingFetchJobs == 1) {
            Q_EMIT fetchingChanged();
        }
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            reply->deleteLater();
            --m_pendingFetchJobs;
            if (reply->error() != QNetworkReply::NoError) {
                qWarning() << reply->errorString();
                return; // TODO with a single error during a full fetch we must not purge
            }

            const auto alertIds = QJsonDocument::fromJson(reply->readAll()).array();
            for (const auto &alertId : alertIds) {
                const auto id = alertId.toString();
                m_fetchedAlertIds.push_back(id);
                fetchAlert(id);
            }

            if (m_pendingFetchJobs == 0) {
                purgeAlerts();
            }
        });
    }

    if (m_subMgr->subscriptions().empty()) {
        purgeAlerts();
    }
}

void AlertsManager::purgeAlerts()
{
    qDebug() << "fetch done, purging leftovers";
    std::sort(m_fetchedAlertIds.begin(), m_fetchedAlertIds.end());
    for (auto it = m_alerts.begin(); it != m_alerts.end();) {
        if (std::binary_search(m_fetchedAlertIds.begin(), m_fetchedAlertIds.end(), (*it).id)) {
            ++it;
            continue;
        }
        const auto row = std::distance(m_alerts.begin(), it);
        beginRemoveRows({}, row, row);
        if ((*it).notification) {
            (*it).notification->close();
        }
        QFile::remove(basePath() + (*it).id + QLatin1String(".xml"));
        it = m_alerts.erase(it);
        endRemoveRows();
    }
    Q_EMIT fetchingChanged();
}

void AlertsManager::removeAlert(const QString &id)
{
    const auto it = std::lower_bound(m_alerts.begin(), m_alerts.end(), id);
    if (it == m_alerts.end() || (*it).id != id) {
        return;
    }

    const auto row = std::distance(m_alerts.begin(), it);
    beginRemoveRows({}, row, row);
    QFile::remove(basePath() + (*it).id + QLatin1String(".xml"));
    m_alerts.erase(it);
    endRemoveRows();
}

int AlertsManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_alerts.size();
}

QVariant AlertsManager::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index)) {
        return {};
    }

    const auto &info = m_alerts[index.row()];
    switch (role) {
        case AlertRole:
            return QVariant::fromValue(info.alertData);
        case AlertInfoRole:
            return QVariant::fromValue(info.info());
        case OnsetTimeRole:
            return info.onsetTime();
        case Qt::DisplayRole:
            return info.alertData.identifier();
    }

    return {};
}

QHash<int, QByteArray> AlertsManager::roleNames() const
{
    auto n = QAbstractListModel::roleNames();
    n.insert(AlertRole, "alert");
    n.insert(AlertInfoRole, "alertInfo");
    return n;
}

AlertElement& AlertsManager::addAlert(AlertElement &&e)
{
    if (e.alert().messageType() == KWeatherCore::CAPAlertMessage::MessageType::Update ||
        e.alert().messageType() == KWeatherCore::CAPAlertMessage::MessageType::Cancel) {
        const auto alert = e.alert();
        for (const auto &ref : alert.references()) {
            const auto it = std::find_if(m_alerts.begin(), m_alerts.end(), [&ref](const auto &alert) {
                return alert.alert().ownReference() == ref;
            });
            if (it == m_alerts.end()) {
                continue;
            }
            qDebug() << "found existing alert that is being updated!" << (*it).id;
            const auto row = std::distance(m_alerts.begin(), it);
            beginRemoveRows({}, row, row);
            // move notification to the updated message, so it gets reused/updated when still active
            if (!e.notification) {
                e.notification.swap((*it).notification);
            }
            if ((*it).notification) {
                (*it).notification->close();
            }
            QFile::remove(basePath() + (*it).id + QLatin1String(".xml")); // ### do we need to ensure we are not reloading this one?
            m_alerts.erase(it);
            endRemoveRows();
        }
    }

    auto it = std::lower_bound(m_alerts.begin(), m_alerts.end(), e);
    if (it != m_alerts.end() && (*it).id == e.id) {
        (*it).alertData = std::move(e.alertData);
        const auto idx = index(std::distance(m_alerts.begin(), it), 0);
        Q_EMIT dataChanged(idx, idx);
        return (*it);
    } else {
        const auto row = std::distance(m_alerts.begin(), it);
        beginInsertRows({}, row, row);
        auto &alert = *m_alerts.insert(it, std::move(e));
        endInsertRows();
        return alert;
    }
}

struct {
    KWeatherCore::CAPAlertInfo::Severity severity;
    const char *eventName;
} static constexpr const notification_map[] = {
    { KWeatherCore::CAPAlertInfo::Severity::Extreme, "extreme-alert" },
    { KWeatherCore::CAPAlertInfo::Severity::Severe, "severe-alert" },
    { KWeatherCore::CAPAlertInfo::Severity::Moderate, "moderate-alert" },
    { KWeatherCore::CAPAlertInfo::Severity::Minor, "minor-alert" },
};

void AlertsManager::showNotification(AlertElement &e)
{
    // cancellations: close of we have a notification open, ignore otherwise
    if (e.alertData.messageType() == KWeatherCore::CAPAlertMessage::MessageType::Cancel) {
        if (e.notification) {
            e.notification->close();
        }
        return;
    }

    const auto info = e.info();
    for (const auto &m : notification_map) {
        if (m.severity == info.severity()) {
            if (!e.notification) {
                auto n = new KNotification(QLatin1String(m.eventName));
                e.notification = n;
                auto viewAction = n->addDefaultAction(i18nc("@action:button", "Show Alert Details"));
                connect(viewAction, &KNotificationAction::activated, this, [this, n]() {
                    notificationActivated(n);
                });
                [[maybe_unused]] auto dismissAction = n->addAction(i18nc("@action:button", "Dismiss"));
                connect(e.notification.data(), &KNotification::closed, this, [this]() {
                    Q_EMIT notificationClosed();
                });
            }
            e.notification->setTitle(info.event());
            e.notification->setText(info.description());
            e.notification->setIconName(CAPUtil::categoriesIconName(info.categories()));
            if (info.severity() == KWeatherCore::CAPAlertInfo::Severity::Extreme) {
                e.notification->setFlags(KNotification::Persistent);
            }
            e.notification->setHint(QStringLiteral("x-kde-visibility"), QStringLiteral("public"));
            e.notification->sendEvent();
            break;
        }
    }
}

void AlertsManager::notificationActivated(const KNotification* notification)
{
    for (const auto &alert : m_alerts) {
        if (alert.notification == notification) {
            Q_EMIT showAlert(alert.id);
            return;
        }
    }
    qWarning() << "Notification for unknown alert activated?";
}

KPublicAlerts::AlertElement AlertsManager::alertById(const QString &id) const
{
    const auto it = std::lower_bound(m_alerts.begin(), m_alerts.end(), id);
    if (it == m_alerts.end() && (*it).id != id) {
        return {};
    }

    return (*it);
}

bool AlertsManager::isFetching() const
{
    return m_pendingFetchJobs > 0;
}

[[nodiscard]] static bool hasGeometry(const KWeatherCore::CAPAlertInfo &info)
{
    return std::ranges::any_of(info.areas(), [](const auto &area) { return !area.polygons().empty() || !area.circles().empty(); });
}

bool AlertsManager::intersectsSubscribedArea(const AlertElement &e) const
{
    const auto alertInfo = e.info();
    if (hasGeometry(alertInfo)) {
        return intersectsSubscribedArea(alertInfo);
    }
    return std::ranges::any_of(e.alertData.alertInfos(), [this](const auto &info) { return intersectsSubscribedArea(info); });
}

bool AlertsManager::intersectsSubscribedArea(const KWeatherCore::CAPAlertInfo &alertInfo) const
{
    for (const auto &area : alertInfo.areas()) {
        for (const auto &poly : area.polygons()) {
            for (const auto &sub : m_subMgr->subscriptions()) {
                if (GeoMath::intersects(poly, sub.m_boundingBox)) {
                    return true;
                }
            }
        }
        for (const auto &circle : area.circles()) {
            for (const auto &sub : m_subMgr->subscriptions()) {
                if (GeoMath::intersects(circle, sub.m_boundingBox)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void AlertsManager::scheduleExpire()
{
    QDateTime dt;
    for (const auto &alert : m_alerts) {
        dt = dt.isValid() ? std::min(alert.expireTime(), dt) : alert.expireTime();
    }
    if (!dt.isValid()) {
        return;
    }
    qDebug() << "next expiry:" << dt;
    m_expireTimer.start(std::max(std::chrono::seconds(QDateTime::currentDateTime().secsTo(dt)), std::chrono::seconds(60)));
}

void AlertsManager::purgeExpired()
{
    qDebug() << "purging expired alerts";
    for (auto it = m_alerts.begin(); it != m_alerts.end();) {
        if ((*it).isExpired()) {
            const auto row = std::distance(m_alerts.begin(), it);
            beginRemoveRows({}, row, row);
            it = m_alerts.erase(it);
            // TODO delete alert file
            endRemoveRows();
        } else {
            ++it;
        }
    }
    scheduleExpire();
}

bool AlertsManager::hasPendingNotifications() const
{
    return std::any_of(m_alerts.begin(), m_alerts.end(), [](const auto &alert) { return alert.notification; });
}

#include "moc_alertsmanager.cpp"
