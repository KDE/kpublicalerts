/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_SUBSCRIPTION_H
#define KPUBLICALERTS_SUBSCRIPTION_H

#include <QDateTime>
#include <QMetaType>
#include <QRectF>
#include <QSettings>
#include <QString>
#include <QUuid>

namespace KPublicAlerts {

/** A area of interest we are subscribed to for push notifications. */
class Subscription
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER m_name)
    Q_PROPERTY(bool isSubscribed READ isSubscribed)
    Q_PROPERTY(bool awaitsConfirmation READ awaitsConfirmation)

public:
    inline bool operator<(const Subscription &other) const
    {
        return m_id < other.m_id;
    }
    inline bool operator<(const QString &id) const
    {
        return m_id < id;
    }

    [[nodiscard]] bool isSubscribed() const;
    [[nodiscard]] bool awaitsConfirmation() const;

    static Subscription load(const QString &id, QSettings &settings);
    void store(QSettings &settings);

    QString m_id;
    QString m_name;
    QUuid m_subscriptionId;
    QString m_notificationEndpoint;
    QRectF m_boundingBox;
    QDateTime m_lastHeartbeat;
    QString m_pendingConfirmation;
};

}

Q_DECLARE_METATYPE(KPublicAlerts::Subscription)

#endif // KPUBLICALERTS_SUBSCRIPTION_H
