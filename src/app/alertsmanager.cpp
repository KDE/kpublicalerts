/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "alertsmanager.h"
#include "caputil.h"

#include <KWeatherCore/AlertInfo>
#include <KWeatherCore/CAPParser>

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
    return !alertData.infoVec().empty();
}

bool AlertElement::isExpired() const
{
    // TODO check all alert info elements
    return alertData.infoVec().at(0).expireTime().isValid() && alertData.infoVec().at(0).expireTime() < QDateTime::currentDateTime();
}

KWeatherCore::AlertInfo AlertElement::info() const
{
    // TODO also handle alert message that contain multiple infos for the same language
    for (const auto &uiLang : QLocale().uiLanguages()) {
        // exact match
        for (const auto &info : alertData.infoVec()) {
            if (info.language().compare(uiLang, Qt::CaseInsensitive) == 0) {
                return info;
            }
        }
        // language-only match
        for (const auto &info : alertData.infoVec()) {
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

    return alertData.infoVec()[0];
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
            QFile::remove(it.filePath());
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

    auto reply = m_nam->get(QNetworkRequest(QUrl(QLatin1String("http://localhost:8000/aggregator/alert/") + id))); // TODO hardcoded URL
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

void AlertsManager::fetchAll()
{
    QUrl url(QLatin1String("http://localhost:8000/aggregator/alerts")); // TODO hardcoded URL
    QUrlQuery query;
    // TODO
    query.addQueryItem(QStringLiteral("minlat"), QString::number(48.0));
    query.addQueryItem(QStringLiteral("maxlat"), QString::number(54.0));
    query.addQueryItem(QStringLiteral("minlon"), QString::number(6.0));
    query.addQueryItem(QStringLiteral("maxlon"), QString::number(14.0));
    url.setQuery(query);

    auto reply = m_nam->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << reply->errorString();
            return;
        }

        const auto alertIds = QJsonDocument::fromJson(reply->readAll()).array();
        for (const auto &alertId : alertIds) {
            fetchAlert(alertId.toString());
        }
    });
}

void AlertsManager::removeAlert(const QString &id)
{
    const auto it = std::lower_bound(m_alerts.begin(), m_alerts.end(), id);
    if (it == m_alerts.end() || (*it).id != id) {
        return;
    }

    const auto row = std::distance(m_alerts.begin(), it);
    beginRemoveRows({}, row, row);
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
    KWeatherCore::AlertInfo::Severity severity;
    const char *eventName;
} static constexpr const notification_map[] = {
    { KWeatherCore::AlertInfo::Severity::Extreme, "extreme-alert" },
    { KWeatherCore::AlertInfo::Severity::Severe, "severe-alert" },
    { KWeatherCore::AlertInfo::Severity::Moderate, "moderate-alert" },
    { KWeatherCore::AlertInfo::Severity::Minor, "minor-alert" },
};

void AlertsManager::showNotification(const AlertElement &e)
{
    if (e.alertData.msgType() != KWeatherCore::AlertEntry::MsgType::Alert || e.alertData.status() != KWeatherCore::AlertEntry::Status::Actual) {
        return;
    }

    const auto info = e.info();
    for (const auto &m : notification_map) {
        if (m.severity == info.severity()) {
            auto n = new KNotification(QLatin1String(m.eventName));
            n->setTitle(info.event());
            n->setText(info.description());
            n->setIconName(CAPUtil::categoriesIconName(info.categories()));
            if (info.severity() == KWeatherCore::AlertInfo::Severity::Extreme || info.severity() == KWeatherCore::AlertInfo::Severity::Severe) {
                n->setFlags(KNotification::Persistent);
            }
            n->setHint(QStringLiteral("x-kde-visibility"), QStringLiteral("public"));
            n->sendEvent();
            // TODO react to activation with showing the details page
            break;
        }
    }
}
