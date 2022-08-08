/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "subscriptionmanager.h"

using namespace KPublicAlerts;

SubscriptionManager::SubscriptionManager(QObject *parent)
    : QObject(parent)
{
}

SubscriptionManager::~SubscriptionManager() = default;
