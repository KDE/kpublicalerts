/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_SUBSCRIPTIONMANAGER_H
#define KPUBLICALERTS_SUBSCRIPTIONMANAGER_H

#include "subscription.h"

#include <KUnifiedPush/Connector>

#include <QAbstractListModel>

#include <vector>

class QNetworkAccessManager;
class QSettings;
class QTimer;

namespace KPublicAlerts {

class SubscriptionManager : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    explicit SubscriptionManager(QObject *parent = nullptr);
    ~SubscriptionManager();

    void setNetworkAccessManager(QNetworkAccessManager *nam);

    enum Role {
        SubscriptionRole = Qt::UserRole,
    };

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex &parent = {}) override;

    // TODO temporary for development
    Q_INVOKABLE void addSubscription(float lat, float lon, float radius, const QString &name);

    const std::vector<Subscription> &subscriptions() const;

Q_SIGNALS:
    void alertAdded(const QString &id);
    void alertRemoved(const QString &id);
    void countChanged();

private:
    void doSubscribeAll();
    void doSubscribeOne(const Subscription &sub);
    void doUnsubscribeOne(const Subscription &sub);
    void doRemoveOne(const QString &id);

    void storeSubscriptionIds(QSettings &settings);

    void checkHeartbeat();
    void fetchVapidKey();

    QNetworkAccessManager *m_nam = nullptr;
    KUnifiedPush::Connector m_connector;
    QTimer *m_heartbeatTimer = nullptr;

    std::vector<Subscription> m_subscriptions;
};

}

#endif // KPUBLICALERTS_SUBSCRIPTIONMANAGER_H
