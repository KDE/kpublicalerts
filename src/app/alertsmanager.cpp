/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "alertsmanager.h"
#include "caputil.h"
#include "restapi.h"
#include "subscriptionmanager.h"

#include <KWeatherCore/CAPParser>

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

bool AlertElement::isExpired() const
{
    // TODO check all alert info elements
    return alertData.alertInfos().at(0).expireTime().isValid() && alertData.alertInfos().at(0).expireTime() < QDateTime::currentDateTime();
}

KWeatherCore::CAPAlertMessage AlertElement::alert() const
{
    return alertData;
}

KWeatherCore::CAPAlertInfo AlertElement::info() const
{
    // TODO also handle alert message that contain multiple infos for the same language
    for (const auto &uiLang : QLocale().uiLanguages()) {
        // exact match
        for (const auto &info : alertData.alertInfos()) {
            if (info.language().compare(uiLang, Qt::CaseInsensitive) == 0) {
                return info;
            }
        }
        // language-only match
        for (const auto &info : alertData.alertInfos()) {
            const auto lang = info.language();
            QStringView l1(lang);
            if (auto idx = l1.indexOf(QLatin1Char('-')); idx > 0) {
                l1 = l1.left(idx);
            }
            QStringView l2(uiLang);
            if (auto idx = l2.indexOf(QLatin1Char('-')); idx > 0) {
                l2 = l2.left(idx);
            }
            if (l1.compare(l2, Qt::CaseInsensitive) == 0) {
                return info;
            }
        }
    }

    return alertData.alertInfos()[0];
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
}

AlertsManager::~AlertsManager() = default;

void AlertsManager::setNetworkAccessManager(QNetworkAccessManager *nam)
{
    m_nam = nam;
}

void AlertsManager::fetchAlert(const QString &id)
{
    if (const auto it = std::lower_bound(m_alerts.begin(), m_alerts.end(), id); it != m_alerts.end() && (*it).id == id) {
        qDebug() << "alert" << id << "already known, not fetching";
        return;
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

        const auto path = basePath();
        QDir().mkpath(path);
        if (QFile f(path + id + QLatin1String(".xml")); f.open(QFile::WriteOnly)) {
            f.write(capData);
        } else {
            qWarning() << f.fileName() << f.errorString();
        }

        showNotification(e);
        addAlert(std::move(e));
    });
}

void AlertsManager::fetchAll(KPublicAlerts::SubscriptionManager *subscriptions)
{
    for (auto i = 0; i < subscriptions->rowCount(); ++i) {
        const auto subscription = subscriptions->index(i, 0).data(SubscriptionManager::SubscriptionRole).value<Subscription>();
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

    if (subscriptions->rowCount() == 0) {
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

void AlertsManager::addAlert(AlertElement &&e)
{
    auto it = std::lower_bound(m_alerts.begin(), m_alerts.end(), e);
    if (it != m_alerts.end() && (*it).id == e.id) {
        (*it).alertData = std::move(e.alertData);
        const auto idx = index(std::distance(m_alerts.begin(), it), 0);
        Q_EMIT dataChanged(idx, idx);
    } else {
        const auto row = std::distance(m_alerts.begin(), it);
        beginInsertRows({}, row, row);
        m_alerts.insert(it, std::move(e));
        endInsertRows();
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

void AlertsManager::showNotification(const AlertElement &e)
{
    if (e.alertData.messageType() != KWeatherCore::CAPAlertMessage::MessageType::Alert || e.alertData.status() != KWeatherCore::CAPAlertMessage::Status::Actual) {
        return;
    }

    const auto info = e.info();
    for (const auto &m : notification_map) {
        if (m.severity == info.severity()) {
            auto n = new KNotification(QLatin1String(m.eventName));
            n->setTitle(info.event());
            n->setText(info.description());
            n->setIconName(CAPUtil::categoriesIconName(info.categories()));
            if (info.severity() == KWeatherCore::CAPAlertInfo::Severity::Extreme) {
                n->setFlags(KNotification::Persistent);
            }
            n->setHint(QStringLiteral("x-kde-visibility"), QStringLiteral("public"));
            n->setDefaultAction(i18n("Show alert details"));
            n->setActions({i18n("Dismiss")});
            const auto id = e.id;
            connect(n, qOverload<uint>(&KNotification::activated), this, [this, id](uint action) {
                if (action == 0) {
                    Q_EMIT showAlert(id);
                }
            });
            n->sendEvent();
            break;
        }
    }
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
