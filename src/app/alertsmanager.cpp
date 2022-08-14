/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "alertsmanager.h"

#include <KWeatherCore/AlertInfo>
#include <KWeatherCore/CAPParser>

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

using namespace KPublicAlerts;

AlertsManager::AlertsManager(QObject* parent)
    : QAbstractListModel(parent)
{
}

AlertsManager::~AlertsManager() = default;

void AlertsManager::setNetworkAccessManager(QNetworkAccessManager *nam)
{
    m_nam = nam;
}

void AlertsManager::addAlert(const QString &id)
{
    auto reply = m_nam->get(QNetworkRequest(QUrl(QLatin1String("http://localhost:8000/aggregator/alert/") + id))); // TODO hardcoded URL
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << reply->errorString();
            return;
        }
        KWeatherCore::CAPParser p(reply->readAll());

        AlertElement e;
        e.alertData = p.parse();
        if (e.alertData.infoVec().at(0).expireTime().isValid() && e.alertData.infoVec().at(0).expireTime() < QDateTime::currentDateTime()) {
            qDebug() << "dropping expired alert:" << e.alertData.identifier();
            return;
        }

        auto it = std::lower_bound(m_alerts.begin(), m_alerts.end(), e);
        beginInsertRows({}, std::distance(m_alerts.begin(), it), std::distance(m_alerts.begin(), it));
        m_alerts.insert(it, std::move(e));
        endInsertRows();
    });
}

void AlertsManager::fetchAll()
{
    auto reply = m_nam->get(QNetworkRequest(QUrl(QLatin1String("http://localhost:8000/aggregator/alerts")))); // TODO hardcoded URL
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << reply->errorString();
            return;
        }

        const auto alertIds = QJsonDocument::fromJson(reply->readAll()).array();
        for (const auto &alertId : alertIds) {
            addAlert(alertId.toString());
        }
    });
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
        {
            for (const auto &uiLang : QLocale().uiLanguages()) {
                // exact match
                for (const auto &info : info.alertData.infoVec()) {
                    if (info.language().compare(uiLang, Qt::CaseInsensitive) == 0) {
                        return QVariant::fromValue(info);
                    }
                }
                // language-only match
                for (const auto &info : info.alertData.infoVec()) {
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
                        return QVariant::fromValue(info);
                    }
                }
            }
            return QVariant::fromValue(info.alertData.infoVec()[0]);
        }
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
