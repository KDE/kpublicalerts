/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "subscription.h"

using namespace Qt::Literals;
using namespace KPublicAlerts;

bool Subscription::isSubscribed() const
{
    return !m_subscriptionId.isNull();
}

bool Subscription::awaitsConfirmation() const
{
    return !m_pendingConfirmation.isEmpty();
}

Subscription Subscription::load(const QString &id, QSettings &settings)
{
    Subscription s;
    settings.beginGroup("Subscription-"_L1 + id);
    s.m_id = id;
    s.m_subscriptionId = QUuid::fromString(settings.value("SubscriptionId"_L1).value<QString>());
    s.m_name = settings.value("Name"_L1).value<QString>();
    s.m_boundingBox = settings.value("BoundingBox"_L1).value<QRectF>();
    s.m_notificationEndpoint = settings.value("NotificationEndpoint"_L1).value<QString>();
    s.m_lastHeartbeat = settings.value("LastHeartbeat"_L1).value<QDateTime>();
    s.m_pendingConfirmation = settings.value("m_pendingConfirmation"_L1).value<QString>();
    settings.endGroup();
    return s;
}

void Subscription::store(QSettings &settings)
{
    settings.beginGroup("Subscription-"_L1 + m_id);
    settings.setValue("Name"_L1, m_name);
    settings.setValue("SubscriptionId"_L1, m_subscriptionId.toString(QUuid::WithoutBraces));
    settings.setValue("BoundingBox"_L1, m_boundingBox);
    settings.setValue("NotificationEndpoint"_L1, m_notificationEndpoint);
    settings.setValue("LastHeartbeat"_L1, m_lastHeartbeat);
    settings.setValue("PendingConfirmation"_L1, m_pendingConfirmation);
    settings.endGroup();
}

#include "moc_subscription.cpp"
