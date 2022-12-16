/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_APPLICATION_H
#define KPUBLICALERTS_APPLICATION_H

#include "alertsmanager.h"
#include "subscriptionmanager.h"

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QObject>

class QQmlApplicationEngine;

namespace KPublicAlerts {

/** Manages the main application state.
 *  Mainly for distinguishing push notification activation and UI activation.
 */
class Application : public QObject
{
    Q_OBJECT
public:
    Application(QObject *parent = nullptr);
    ~Application();

    void showUi();
    void processDBusActivation(const QStringList &args);

private:
    QQmlApplicationEngine *m_qmlAppEngine = nullptr;

    QNetworkAccessManager m_nam;
    QNetworkDiskCache m_namDiskCache;
    SubscriptionManager m_subscriptionMgr;
    AlertsManager m_alertsMgr;
};

}

#endif // KPUBLICALERTS_APPLICATION_H
