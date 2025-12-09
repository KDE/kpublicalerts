/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.coreaddons as KCoreAddons
import org.kde.guiaddons as KGuiAddons
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.notification as KNotification
import org.kde.weathercore
import org.kde.publicalerts

Kirigami.ApplicationWindow {
    id: root

    title: i18nc("@title:window", "Public Alerts")

    // HACK work around Kirigami KF 6.17 mobile mode regression
    pageStack {
        columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn
        globalToolBar {
            style: Kirigami.ApplicationHeaderStyle.ToolBar
            showNavigationButtons: pageStack.currentIndex > 0 || pageStack.layers.depth > 1 ? Kirigami.ApplicationHeaderStyle.ShowBackButton : 0
        }
    }

    header: Kirigami.InlineMessage {
        id: notificationPermissionWarning
        text: i18n("Missing notification permission!")
        type: Kirigami.MessageType.Error
        position: Kirigami.InlineMessage.Header
        visible: !KNotification.NotificationPermission.checkPermission()

        function permissionCallback(success) {
            notificationPermissionWarning.visible = !success;
        }
        actions: [
            Kirigami.Action {
                icon.name: "notifications"
                text: i18n("Request permission…")
                onTriggered: KNotification.NotificationPermission.requestPermission(notificationPermissionWarning.permissionCallback)
            }
        ]
    }

    Component {
        id: subscriptionsPage
        SubscriptionsPage {}
    }

    Component {
        id: aboutPage
        FormCard.AboutPage {
            aboutData: KCoreAddons.AboutData
        }
    }

    // should be in CAPUtil in C++, but that would require KF5::ConfigWidgets for the color scheme...
    function severityTextColor(severity: int): color
    {
        switch(severity) {
            case CAPAlertInfo.Extreme: return Kirigami.Theme.negativeTextColor;
            case CAPAlertInfo.Severe: return Kirigami.Theme.neutralTextColor;
            case CAPAlertInfo.Moderate: return KGuiAddons.ColorUtils.mix(Kirigami.Theme.neutralTextColor, Kirigami.Theme.positiveTextColor, 0.35);
            case CAPAlertInfo.Minor: return Kirigami.Theme.positiveTextColor;
        }
        return Kirigami.Theme.textColor;
    }

    globalDrawer: Kirigami.GlobalDrawer {
        title: i18nc("@title:window", "Public Alerts")
        titleIcon: "weather-storm"
        isMenu: true
        actions: [
            Kirigami.Action {
                text: i18nc("@action:inmenu", "Areas of Interest…")
                icon.name: "map-globe"
                enabled: applicationWindow().pageStack.layers.depth <= 1
                onTriggered: applicationWindow().pageStack.layers.push(subscriptionsPage)
            },
            Kirigami.Action {
                text: i18nc("@action:inmenu", "About")
                icon.name: "help-about-symbolic"
                onTriggered: applicationWindow().pageStack.pushDialogLayer(aboutPage)
            }
        ]
    }

    pageStack.initialPage: AlertsPage {}
}
