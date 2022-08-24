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

struct AlertElement {
    QString id;
    KWeatherCore::AlertEntry alertData;

    bool operator<(const AlertElement &other) const;
    bool operator<(const QString &otherId) const;

    bool isValid() const;
    bool isExpired() const;
};

class AlertsManager : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AlertsManager(QObject *parent = nullptr);
    ~AlertsManager();

    void setNetworkAccessManager(QNetworkAccessManager *nam);

    void fetchAlert(const QString &id);
    Q_INVOKABLE void fetchAll(); // TODO testing only, needs to take bbox argument

    enum {
        AlertRole = Qt::UserRole,
        AlertInfoRole,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex & index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    void addAlert(AlertElement &&e);

    std::vector<AlertElement> m_alerts;
    QNetworkAccessManager *m_nam = nullptr;
};

}

#endif // KPUBLICALERTS_ALERTSMANAGER_H
