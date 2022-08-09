/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_SUBSCRIPTIONMANAGER_H
#define KPUBLICALERTS_SUBSCRIPTIONMANAGER_H

#include <KUnifiedPush/Connector>

#include <QObject>

class QNetworkAccessManager;

namespace KPublicAlerts {

class SubscriptionManager : public QObject
{
public:
    explicit SubscriptionManager(QObject *parent = nullptr);
    ~SubscriptionManager();

    void setNetworkAccessManager(QNetworkAccessManager *nam);

private:
    QNetworkAccessManager *m_nam = nullptr;
    KUnifiedPush::Connector m_connector;
};

}

#endif // KPUBLICALERTS_SUBSCRIPTIONMANAGER_H
