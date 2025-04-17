/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "application.h"

#include "alertssortproxymodel.h"
#include "areamodel.h"
#include "caputil.h"

#include <KWeatherCore/CAPAlertMessage>
#include <KWeatherCore/CAPAlertInfo>

#include <KAboutData>
#include <KLocalizedQmlContext>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>

using namespace KPublicAlerts;

Application::Application(QObject *parent)
    : QObject(parent)
{
    m_nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    m_nam.enableStrictTransportSecurityStore(true, QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1String("/hsts/"));
    m_nam.setStrictTransportSecurityEnabled(true);
    m_namDiskCache.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1String("/nam/"));
    m_nam.setCache(&m_namDiskCache);

    m_subscriptionMgr.setNetworkAccessManager(&m_nam);
    m_alertsMgr.setNetworkAccessManager(&m_nam);
    m_alertsMgr.setSubscriptionManager(&m_subscriptionMgr);
    connect(&m_subscriptionMgr, &SubscriptionManager::alertAdded, &m_alertsMgr, &AlertsManager::fetchAlert);
    connect(&m_subscriptionMgr, &SubscriptionManager::alertRemoved, &m_alertsMgr, &AlertsManager::removeAlert);
    connect(&m_subscriptionMgr, &SubscriptionManager::rowsInserted, &m_alertsMgr, &AlertsManager::fetchAll);
    connect(&m_subscriptionMgr, &SubscriptionManager::rowsRemoved, &m_alertsMgr, &AlertsManager::fetchAll);
    connect(&m_subscriptionMgr, &SubscriptionManager::unhandledPushNotifications, &m_alertsMgr, &AlertsManager::fetchAll);

    connect(&m_alertsMgr, &AlertsManager::showAlert, this, &Application::showUi);
    connect(&m_alertsMgr, &AlertsManager::notificationClosed, this, &Application::maybeQuit, Qt::QueuedConnection);
    connect(&m_alertsMgr, &AlertsManager::fetchingChanged, this, &Application::maybeQuit, Qt::QueuedConnection);
}

Application::~Application() = default;

void Application::showUi()
{
    if (m_qmlAppEngine) {
        return;
    }

    qRegisterMetaType<KPublicAlerts::AlertElement>();
    qmlRegisterType<KPublicAlerts::AlertsSortProxyModel>("org.kde.publicalerts", 1, 0, "AlertsSortProxyModel");
    qmlRegisterType<KPublicAlerts::AreaModel>("org.kde.publicalerts", 1, 0, "AreaModel");
    qmlRegisterSingletonType("org.kde.publicalerts", 1, 0, "CAPUtil", [](QQmlEngine *engine, QJSEngine*) -> QJSValue {
        return engine->toScriptValue(CAPUtil());
    });

    qmlRegisterSingletonInstance("org.kde.publicalerts", 1, 0, "SubscriptionManager", &m_subscriptionMgr);
    qmlRegisterSingletonInstance("org.kde.publicalerts", 1, 0, "AlertsManager", &m_alertsMgr);

    m_qmlAppEngine = new QQmlApplicationEngine(this);

    KLocalization::setupLocalizedContext(m_qmlAppEngine);

    m_qmlAppEngine->loadFromModule("org.kde.publicalerts", "Main");
}

void Application::processDBusActivation(const QStringList &args)
{
    if (!args.contains(QLatin1String("--dbus-activated"))) {
        showUi();
    }
}

void Application::maybeQuit()
{
    if (!m_alertsMgr.hasPendingNotifications() && !m_alertsMgr.isFetching() && !m_qmlAppEngine) {
        QCoreApplication::quit();
    }
}

#include "moc_application.cpp"
