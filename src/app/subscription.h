/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_SUBSCRIPTION_H
#define KPUBLICALERTS_SUBSCRIPTION_H

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
public:
    inline bool operator<(const Subscription &other) const
    {
        return m_id < other.m_id;
    }
    inline bool operator<(const QString &id) const
    {
        return m_id < id;
    }

    static Subscription load(const QString &id, QSettings &settings);
    void store(QSettings &settings);

    QString m_id;
    QString m_name;
    QUuid m_subscriptionId;
    QString m_notificationEndpoint;
    QRectF m_boundingBox;
};

}

Q_DECLARE_METATYPE(KPublicAlerts::Subscription)

#endif // KPUBLICALERTS_SUBSCRIPTION_H
