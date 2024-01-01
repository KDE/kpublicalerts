/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.weathercore
import org.kde.publicalerts

Kirigami.ApplicationWindow {
    id: root

    title: i18n("Public Alerts")

    Component {
        id: subscriptionsPage
        SubscriptionsPage {}
    }

    Component {
        id: aboutPage
        Kirigami.AboutPage {
            aboutData: AboutData
        }
    }

    // should be in CAPUtil in C++, but that would require KF5::ConfigWidgets for the color scheme...
    function severityTextColor(severity)
    {
        switch(severity) {
            case CAPAlertInfo.Extreme: return Kirigami.Theme.negativeTextColor;
            case CAPAlertInfo.Severe: return Kirigami.Theme.neutralTextColor;
            case CAPAlertInfo.Moderate: return CAPUtil.colorMix(Kirigami.Theme.neutralTextColor, Kirigami.Theme.positiveTextColor, 0.35);
            case CAPAlertInfo.Minor: return Kirigami.Theme.positiveTextColor;
        }
        return Kirigami.Theme.textColor;
    }

    globalDrawer: Kirigami.GlobalDrawer {
        title: i18n("Public Alerts")
        titleIcon: "weather-storm"
        isMenu: true
        actions: [
            Kirigami.Action {
                text: i18n("Areas of interest...")
                icon.name: "map-globe"
                onTriggered: applicationWindow().pageStack.push(subscriptionsPage)
            },
            Kirigami.Action {
                text: i18n("About")
                icon.name: "help-about"
                onTriggered: {
                    if (applicationWindow().pageStack.layers.depth < 2) {
                        applicationWindow().pageStack.layers.push(aboutPage)
                    }
                }
            }
        ]
    }

    pageStack.initialPage: AlertsPage {}
}
