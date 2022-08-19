/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "subscription.h"

using namespace KPublicAlerts;

Subscription Subscription::load(const QString &id, QSettings &settings)
{
    Subscription s;
    settings.beginGroup(QLatin1String("Subscription-") + id);
    s.m_id = id;
    s.m_subscriptionId = settings.value(QLatin1String("SubscriptionId")).value<QUuid>();
    s.m_name = settings.value(QLatin1String("Name")).value<QString>();
    s.m_boundingBox = settings.value(QLatin1String("BoundingBox")).value<QRectF>();
    s.m_notificationEndpoint = settings.value(QLatin1String("NotificationEndpoint")).value<QString>();
    settings.endGroup();
    return s;
}

void Subscription::store(QSettings &settings)
{
    settings.beginGroup(QLatin1String("Subscription-") + m_id);
    settings.setValue(QLatin1String("Name"), m_name);
    settings.setValue(QLatin1String("SubscriptionId"), m_subscriptionId);
    settings.setValue(QLatin1String("BoundingBox"), m_boundingBox);
    settings.setValue(QLatin1String("NotificationEndpoint"), m_notificationEndpoint);
    settings.endGroup();
}
