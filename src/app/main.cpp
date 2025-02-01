/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "application.h"

#include <QCommandLineParser>
#include <QIcon>
#include <QQuickStyle>

#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif

#include <KAboutData>

#ifndef Q_OS_ANDROID
#include <KDBusService>
#endif
#include <KLocalizedString>

#include "version.h"

using namespace Qt::Literals;
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
    QGuiApplication::setApplicationDisplayName(i18nc("@title:window", "Public Alerts"));
    QGuiApplication::setDesktopFileName(QStringLiteral("org.kde.publicalerts"));
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("weather-storm")));

#ifdef Q_OS_ANDROID
    QGuiApplication qtApp(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("Material"));
#else
    QIcon::setFallbackThemeName(QStringLiteral("breeze"));
    QApplication qtApp(argc, argv); // for native file dialogs

    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
#endif

    static auto aboutData = KAboutData::applicationData();
    aboutData.setLicense(KAboutLicense::LGPL_V2, KAboutLicense::OrLaterVersions);
    aboutData.setShortDescription(i18nc("@info", "Weather and emergency alerts"));
    aboutData.setCopyrightStatement(i18nc("@info:credit", "Copyright © 2022 The KDE Community"));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    QCommandLineOption serviceLaunchOpt(u"dbus-activated"_s, u"indicates D-Bus activation (internal)"_s);
    parser.addOption(serviceLaunchOpt);
    QCommandLineOption selfTestOpt(u"self-test"_s, u"for automated testing (internal)"_s);
    parser.addOption(selfTestOpt);
    parser.addVersionOption();
    parser.process(*QCoreApplication::instance());

#ifndef Q_OS_ANDROID
    KDBusService service(KDBusService::Unique);
#endif

    Application app;
    if (!parser.isSet(serviceLaunchOpt)) {
        app.showUi();
    } else {
        QTimer::singleShot(std::chrono::seconds(30), Qt::VeryCoarseTimer, &app, &Application::maybeQuit);
    }


#ifndef Q_OS_ANDROID
    QObject::connect(&service, &KDBusService::activateRequested, &app, &Application::processDBusActivation);
#endif

    if (parser.isSet(selfTestOpt)) {
        QTimer::singleShot(std::chrono::milliseconds(250), &app, &QCoreApplication::quit);
    }

    return qtApp.exec();
}
