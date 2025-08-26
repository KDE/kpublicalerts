/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_SUBSCRIPTIONMANAGER_H
#define KPUBLICALERTS_SUBSCRIPTIONMANAGER_H

#include "subscription.h"

#include <KUnifiedPush/Connector>

#include <QAbstractListModel>

#include <unordered_set>
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
        IsSubscribedRole,
        AwaitsConfirmationRole,
    };

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex &parent = {}) override;

    /** Add a new subscription. */
    Q_INVOKABLE void add(Subscription sub);
    /** Update an existing subscription. */
    Q_INVOKABLE void update(Subscription sub);

    /** Subscription factory function for QML.
     *  TODO: remove this once Subscription is registered as a structurally constructable value type.
     */
    Q_INVOKABLE [[nodiscard]] static Subscription makeSubscription();

    const std::vector<Subscription> &subscriptions() const;

Q_SIGNALS:
    void alertAdded(const QString &id);
    void alertUpdated(const QString &id);
    void alertRemoved(const QString &id);
    void countChanged();

    void unhandledPushNotifications();

private:
    void doSubscribeAll();
    void doSubscribeOne(const Subscription &sub);
    void doUnsubscribeOne(const Subscription &sub);
    void doRemoveOne(const QString &id);

    void storeSubscriptionIds(QSettings &settings);

    void checkHeartbeat();
    void updatePushConnectorRegistration();
    void pushMessageReceived(const QByteArray &msg);
    void fetchVapidKey();

    QNetworkAccessManager *m_nam = nullptr;
    std::unique_ptr<KUnifiedPush::Connector> m_connector;
    QTimer *m_heartbeatTimer = nullptr;

    std::vector<Subscription> m_subscriptions;
    std::unordered_set<QString> m_confirmations;
};

}

#endif // KPUBLICALERTS_SUBSCRIPTIONMANAGER_H
