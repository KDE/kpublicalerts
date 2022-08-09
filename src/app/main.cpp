/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "alertsmanager.h"
#include "subscriptionmanager.h"

#include <QCommandLineParser>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>

#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif

#include <KWeatherCore/AlertEntry>
#include <KWeatherCore/AlertInfo>

#include <KDBusService>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "version.h"

using namespace KPublicAlerts;

int main(int argc, char **argv)
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
#else
    QApplication app(argc, argv);
#endif

    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setApplicationName(QStringLiteral("publicalerts"));
    QCoreApplication::setApplicationVersion(QStringLiteral(KPUBLICALERTS_VERSION_STRING));
    QGuiApplication::setApplicationDisplayName(i18n("Public Alerts"));

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringLiteral("dbus-activated"), QStringLiteral("indicated D-Bus activation (internal)")));
    parser.addVersionOption();
    parser.process(app);

    KDBusService service(KDBusService::Unique);

    QQmlApplicationEngine engine;

    // TODO move to KWeatherCore itself
    qmlRegisterUncreatableType<KWeatherCore::AlertInfo>("org.kde.weathercore", 1, 0, "AlertInfo", {});
    qmlRegisterUncreatableType<KWeatherCore::AlertEntry>("org.kde.weathercore", 1, 0, "AlertEntry", {});

    QNetworkAccessManager nam;
    nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    nam.enableStrictTransportSecurityStore(true, QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1String("/hsts/"));
    nam.setStrictTransportSecurityEnabled(true);
    QNetworkDiskCache namDiskCache;
    namDiskCache.setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1String("/nam/"));
    nam.setCache(&namDiskCache);

    SubscriptionManager subscriptionMgr;
    subscriptionMgr.setNetworkAccessManager(&nam);
    qmlRegisterSingletonInstance("org.kde.publicalerts", 1, 0, "SubscriptionManager", &subscriptionMgr);
    AlertsManager alertsMgr;
    alertsMgr.setNetworkAccessManager(&nam);
    qmlRegisterSingletonInstance("org.kde.publicalerts", 1, 0, "AlertsManager", &alertsMgr);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    return app.exec();
}
