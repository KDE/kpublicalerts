/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_ALERTSMANAGER_H
#define KPUBLICALERTS_ALERTSMANAGER_H

#include <KWeatherCore/AlertEntry>

#include <QAbstractListModel>

class QNetworkAccessManager;

namespace KPublicAlerts {

class AlertsManager : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AlertsManager(QObject *parent = nullptr);
    ~AlertsManager();

    void setNetworkAccessManager(QNetworkAccessManager *nam);

    Q_INVOKABLE void addAlert(const QUrl &capData); // TODO invokable only for testing

    enum {
        AlertRole = Qt::UserRole,
        AlertInfoRole,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex & index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    struct AlertElement {
        // TODO id
        KWeatherCore::AlertEntry alertData;

        inline bool operator<(const AlertElement &other) const
        {
            return alertData.sentTime() > other.alertData.sentTime();
        }
    };

    std::vector<AlertElement> m_alerts;
    QNetworkAccessManager *m_nam = nullptr;
};

}

#endif // KPUBLICALERTS_ALERTSMANAGER_H
