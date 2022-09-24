/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "alertsmanager.h"
#include "alertssortproxymodel.h"
#include "areamodel.h"
#include "caputil.h"
#include "subscriptionmanager.h"

#include <QCommandLineParser>
#include <QIcon>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStandardPaths>

#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif

#include <KWeatherCore/CAPAlertMessage>
#include <KWeatherCore/CAPAlertInfo>

#include <KAboutData>

#ifndef Q_OS_ANDROID
#include <KDBusService>
#endif
#include <KLocalizedContext>
#include <KLocalizedString>

#include "version.h"

using namespace KPublicAlerts;

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif
int main(int argc, char **argv)
{
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setApplicationName(QStringLiteral("publicalerts"));
    QCoreApplication::setApplicationVersion(QStringLiteral(KPUBLICALERTS_VERSION_STRING));
    QGuiApplication::setApplicationDisplayName(i18n("Public Alerts"));
    QGuiApplication::setDesktopFileName(QStringLiteral("org.kde.publicalerts"));
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("weather-storm")));

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("Material"));
#else
    QIcon::setFallbackThemeName(QStringLiteral("breeze"));
    QApplication app(argc, argv); // for native file dialogs

    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
#endif

    static auto aboutData = KAboutData::applicationData();
    aboutData.setLicense(KAboutLicense::LGPL_V2, KAboutLicense::OrLaterVersions);
    aboutData.setShortDescription(i18n("Weather and emergency alerts"));
    aboutData.setCopyrightStatement(i18n("Copyright © 2022 The KDE Community"));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    const auto serviceLaunchOpt = QCommandLineOption(QStringLiteral("dbus-activated"), QStringLiteral("indicated D-Bus activation (internal)"));
    parser.addOption(serviceLaunchOpt);
    parser.addVersionOption();
    parser.process(app);
    qDebug() << parser.isSet(serviceLaunchOpt);

#ifndef Q_OS_ANDROID
    KDBusService service(KDBusService::Unique);
#endif

    QQmlApplicationEngine engine;

    // TODO move to KWeatherCore itself
    qRegisterMetaType<KWeatherCore::CAPAlertMessage>();
    qRegisterMetaType<KWeatherCore::CAPAlertMessage::Status>();
    qRegisterMetaType<KWeatherCore::CAPAlertMessage::MessageType>();
    qRegisterMetaType<KWeatherCore::CAPAlertInfo>();
    qRegisterMetaType<KWeatherCore::CAPAlertInfo::Categories>();
    qRegisterMetaType<KWeatherCore::CAPAlertInfo::ResponseTypes>();
    qRegisterMetaType<KWeatherCore::CAPAlertInfo::Severity>();
    qRegisterMetaType<KWeatherCore::CAPAlertInfo::Urgency>();
    qRegisterMetaType<KWeatherCore::CAPAlertInfo::Certainty>();
    qmlRegisterUncreatableType<KWeatherCore::CAPAlertInfo>("org.kde.weathercore", 1, 0, "CAPAlertInfo", {});
    qmlRegisterUncreatableType<KWeatherCore::CAPAlertMessage>("org.kde.weathercore", 1, 0, "CAPAlertMessage", {});

    qRegisterMetaType<KPublicAlerts::AlertElement>();
    qmlRegisterType<KPublicAlerts::AlertsSortProxyModel>("org.kde.publicalerts", 1, 0, "AlertsSortProxyModel");
    qmlRegisterType<KPublicAlerts::AreaModel>("org.kde.publicalerts", 1, 0, "AreaModel");
    qmlRegisterSingletonType("org.kde.publicalerts", 1, 0, "CAPUtil", [](QQmlEngine *engine, QJSEngine*) -> QJSValue {
        return engine->toScriptValue(CAPUtil());
    });
    qmlRegisterSingletonType("org.kde.publicalerts", 1, 0, "AboutData", [](QQmlEngine *engine, QJSEngine*) -> QJSValue {
        return engine->toScriptValue(aboutData);
    });

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
    QObject::connect(&subscriptionMgr, &SubscriptionManager::alertAdded, &alertsMgr, &AlertsManager::fetchAlert);
    QObject::connect(&subscriptionMgr, &SubscriptionManager::alertRemoved, &alertsMgr, &AlertsManager::removeAlert);
    QObject::connect(&subscriptionMgr, &SubscriptionManager::rowsInserted, &alertsMgr, [&]() { alertsMgr.fetchAll(&subscriptionMgr); });
    QObject::connect(&subscriptionMgr, &SubscriptionManager::rowsRemoved, &alertsMgr, [&]() { alertsMgr.fetchAll(&subscriptionMgr); });

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    return app.exec();
}
