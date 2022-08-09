/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "subscriptionmanager.h"

#include <KUnifiedPush/Connector>

#include <KLocalizedString>

#include <QDebug>

using namespace KPublicAlerts;

SubscriptionManager::SubscriptionManager(QObject *parent)
    : QObject(parent)
    , m_connector(QStringLiteral("org.kde.publicalerts"))
{
    connect(&m_connector, &KUnifiedPush::Connector::endpointChanged, this, [this]() {
        qDebug() << m_connector.endpoint();
    });
    connect(&m_connector, &KUnifiedPush::Connector::messageReceived, this, [this](const QByteArray &msg) {
        qDebug() << msg;
    });

    m_connector.registerClient(i18n("Weater and emergency alert notifications."));
}

SubscriptionManager::~SubscriptionManager() = default;

void SubscriptionManager::setNetworkAccessManager(QNetworkAccessManager *nam)
{
    m_nam = nam;
}
