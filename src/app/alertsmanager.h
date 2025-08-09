/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_ALERTSMANAGER_H
#define KPUBLICALERTS_ALERTSMANAGER_H

#include <KWeatherCore/CAPAlertMessage>
#include <KWeatherCore/CAPAlertInfo>

#include <QAbstractListModel>
#include <QPointer>
#include <QTimer>

class KNotification;
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

    [[nodiscard]] bool isValid() const;
    [[nodiscard]] QDateTime expireTime() const;
    [[nodiscard]] bool isExpired() const;
    [[nodiscard]] QDateTime onsetTime() const;

    [[nodiscard]] KWeatherCore::CAPAlertMessage alert() const;
    [[nodiscard]] KWeatherCore::CAPAlertInfo info() const;

    QPointer<KNotification> notification;
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

    void fetchAlert(const QString &id, bool force = false);
    void removeAlert(const QString &id);

    enum {
        AlertRole = Qt::UserRole,
        AlertInfoRole,
        OnsetTimeRole,
        UserRole,
    };

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex & index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE [[nodiscard]] KPublicAlerts::AlertElement alertById(const QString &id) const;

    [[nodiscard]] bool isFetching() const;
    [[nodiscard]] bool hasPendingNotifications() const;

public Q_SLOTS:
    void fetchAll();

Q_SIGNALS:
    void showAlert(const QString &id);
    void fetchingChanged();
    void notificationClosed();

private:
    AlertElement& addAlert(AlertElement &&e);
    void showNotification(AlertElement &e);
    void notificationActivated(const KNotification *notification);
    void purgeAlerts();
    [[nodiscard]] bool intersectsSubscribedArea(const AlertElement &e) const;

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
