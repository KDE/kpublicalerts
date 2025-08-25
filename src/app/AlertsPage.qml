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

Kirigami.ScrollablePage {
    id: root
    title: i18nc("@title:window", "Active Alerts")
    supportsRefreshing: true

    Component {
        id: alertPage
        AlertPage {}
    }

    ListView {
        id: listView
        clip: true
        model: AlertsSortProxyModel {
            sourceModel: AlertsManager
        }
        delegate: QQC2.ItemDelegate {
            id: delegateRoot
            required property capAlertMessage alert
            required property capAlertInfo alertInfo
            width: ListView.view.width
            contentItem: Kirigami.IconTitleSubtitle {
                title: delegateRoot.alertInfo.headline ? delegateRoot.alertInfo.headline : delegateRoot.alertInfo.event
                subtitle: delegateRoot.alertInfo.description
                icon.name: {
                    if (delegateRoot.alert.messageType === CAPAlertMessage.Cancel || delegateRoot.alertInfo.responseTypes === CAPAlertInfo.AllClear)
                        return "dialog-ok";
                    return CAPUtil.categoriesIconName(delegateRoot.alertInfo.categories);
                }
                icon.color: {
                    if (delegateRoot.alert.messageType === CAPAlertMessage.Cancel || delegateRoot.alertInfo.responseTypes === CAPAlertInfo.AllClear)
                        return Kirigami.Theme.disabledTextColor;
                    if (delegateRoot.alert.status === CAPAlertMessage.Exercise)
                        return Kirigami.Theme.activeTextColor;
                    return applicationWindow().severityTextColor(delegateRoot.alertInfo.severity);
                }
            }
            onClicked: {
                while (applicationWindow().pageStack.depth > 1)
                    applicationWindow().pageStack.pop();
                applicationWindow().pageStack.push(alertPage, { alert: delegateRoot.alert, alertInfo: delegateRoot.alertInfo })
            }
        }

        section.property: "sectionTitle"
        section.delegate:Kirigami.ListSectionHeader {
            required property string section
            text: section
            width: ListView.view.width
        }
        section.criteria: ViewSection.FullString
        section.labelPositioning: ViewSection.CurrentLabelAtStart | ViewSection.InlineLabels

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: listView.count == 0
            text: i18nc("@info:placeholder", "No alerts")
            helpfulAction: Kirigami.Action {
                icon.name: "list-add"
                text: i18nc("@action:button", "Add Area of Interest…")
                onTriggered: applicationWindow().pageStack.layers.push(subscriptionsPage)
                enabled: SubscriptionManager.count == 0
            }
        }
    }

    Connections {
        target: AlertsManager
        function onShowAlert(id) {
            const alert = AlertsManager.alertById(id);
            while (applicationWindow().pageStack.depth > 1) {
                applicationWindow().pageStack.pop();
            }
            applicationWindow().pageStack.push(alertPage, { alert: alert.alert, alertInfo: alert.info });
        }
        function onFetchingChanged() {
            root.refreshing = AlertsManager.fetching
        }
    }

    onRefreshingChanged: {
        if (root.refreshing) {
            AlertsManager.fetchAll();
        }
    }

    Component.onCompleted: {
        AlertsManager.fetchAll();
    }
}
