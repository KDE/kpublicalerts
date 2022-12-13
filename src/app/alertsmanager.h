/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_ALERTSMANAGER_H
#define KPUBLICALERTS_ALERTSMANAGER_H

#include <KWeatherCore/CAPAlertMessage>
#include <KWeatherCore/CAPAlertInfo>

#include <QAbstractListModel>
#include <QTimer>

class QNetworkAccessManager;

namespace KPublicAlerts {

class SubscriptionManager;

class AlertElement {
    Q_GADGET
    Q_PROPERTY(KWeatherCore::CAPAlertMessage alert READ alert)
    Q_PROPERTY(KWeatherCore::CAPAlertInfo info READ info)

public:
    QString id;
    KWeatherCore::CAPAlertMessage alertData;

    bool operator<(const AlertElement &other) const;
    bool operator<(const QString &otherId) const;

    bool isValid() const;
    QDateTime expireTime() const;
    bool isExpired() const;

    KWeatherCore::CAPAlertMessage alert() const;
    KWeatherCore::CAPAlertInfo info() const;
};

class AlertsManager : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool fetching READ isFetching NOTIFY fetchingChanged)
public:
    explicit AlertsManager(QObject *parent = nullptr);
    ~AlertsManager();

    void setNetworkAccessManager(QNetworkAccessManager *nam);
    void setSubscriptionManager(SubscriptionManager *subMgr);

    void fetchAlert(const QString &id);
    void removeAlert(const QString &id);

    enum {
        AlertRole = Qt::UserRole,
        AlertInfoRole,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex & index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE KPublicAlerts::AlertElement alertById(const QString &id) const;

    bool isFetching() const;

public Q_SLOTS:
    void fetchAll();

Q_SIGNALS:
    void showAlert(const QString &id);
    void fetchingChanged();

private:
    void addAlert(AlertElement &&e);
    void showNotification(const AlertElement &e);
    void purgeAlerts();
    bool intersectsSubscribedArea(const AlertElement &e) const;

    void scheduleExpire();
    void purgeExpired();

    std::vector<AlertElement> m_alerts;
    QNetworkAccessManager *m_nam = nullptr;
    SubscriptionManager *m_subMgr = nullptr;
    QTimer m_expireTimer;

    int m_pendingFetchJobs = 0;
    std::vector<QString> m_fetchedAlertIds;
};

}

Q_DECLARE_METATYPE(KPublicAlerts::AlertElement)

#endif // KPUBLICALERTS_ALERTSMANAGER_H
