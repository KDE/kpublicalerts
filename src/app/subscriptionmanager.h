/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_SUBSCRIPTIONMANAGER_H
#define KPUBLICALERTS_SUBSCRIPTIONMANAGER_H

#include <QObject>

namespace KPublicAlerts {

class SubscriptionManager : public QObject
{
public:
    explicit SubscriptionManager(QObject *parent = nullptr);
    ~SubscriptionManager();
};

}

#endif // KPUBLICALERTS_SUBSCRIPTIONMANAGER_H
