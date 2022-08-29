/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_ALERTSMANAGER_H
#define KPUBLICALERTS_ALERTSMANAGER_H

#include <KWeatherCore/AlertEntry>
#include <KWeatherCore/AlertInfo>

#include <QAbstractListModel>

class QNetworkAccessManager;

namespace KPublicAlerts {

class AlertElement {
    Q_GADGET
    Q_PROPERTY(KWeatherCore::AlertEntry alert READ alert)
    Q_PROPERTY(KWeatherCore::AlertInfo info READ info)

public:
    QString id;
    KWeatherCore::AlertEntry alertData;

    bool operator<(const AlertElement &other) const;
    bool operator<(const QString &otherId) const;

    bool isValid() const;
    bool isExpired() const;

    KWeatherCore::AlertEntry alert() const;
    KWeatherCore::AlertInfo info() const;
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
    void removeAlert(const QString &id);

    enum {
        AlertRole = Qt::UserRole,
        AlertInfoRole,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex & index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE KPublicAlerts::AlertElement alertById(const QString &id) const;

Q_SIGNALS:
    void showAlert(const QString &id);

private:
    void addAlert(AlertElement &&e);
    void showNotification(const AlertElement &e);

    std::vector<AlertElement> m_alerts;
    QNetworkAccessManager *m_nam = nullptr;
};

}

Q_DECLARE_METATYPE(KPublicAlerts::AlertElement)

#endif // KPUBLICALERTS_ALERTSMANAGER_H
