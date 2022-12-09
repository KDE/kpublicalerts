/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami

import org.kde.weathercore 1.0
import org.kde.publicalerts 1.0

Kirigami.ScrollablePage {
    id: root
    title: i18n("Current Alerts")
    supportsRefreshing: true

    Component {
        id: alertPage
        AlertPage {}
    }

    ListView {
        id: listView
        model: AlertsSortProxyModel {
            sourceModel: AlertsManager
        }
        delegate: Kirigami.BasicListItem {
            text: model.alertInfo.headline ? model.alertInfo.headline : model.alertInfo.event
            subtitle: model.alertInfo.description
            icon: {
                if (model.alert.messageType == CAPAlertMessage.Cancel)
                    return "dialog-ok";
                return CAPUtil.categoriesIconName(model.alertInfo.categories);
            }
            iconColor: {
                if (model.alert.messageType == CAPAlertMessage.Cancel)
                    return Kirigami.Theme.disabledTextColor;
                if (model.alert.status == CAPAlertMessage.Exercise)
                    return Kirigami.Theme.activeTextColor;
                return applicationWindow().severityTextColor(model.alertInfo.severity);
            }
            onClicked: {
                applicationWindow().pageStack.push(alertPage, { alert: model.alert, alertInfo: model.alertInfo })
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: listView.count == 0
            text: i18n("No alerts")
            helpfulAction: Kirigami.Action {
                icon.name: "list-add"
                text: i18n("Add area of interest...")
                onTriggered: applicationWindow().pageStack.push(subscriptionsPage)
                enabled: SubscriptionManager.count == 0
            }
        }
    }

    Connections {
        target: AlertsManager
        onShowAlert: {
            const alert = AlertsManager.alertById(id);
            while (applicationWindow().pageStack.depth > 1) {
                applicationWindow().pageStack.pop();
            }
            applicationWindow().pageStack.push(alertPage, { alert: alert.alert, alertInfo: alert.info });
        }
        onFetchingChanged: {
            root.refreshing = AlertsManager.fetching
        }
    }

    onRefreshingChanged: {
        if (root.refreshing) {
            AlertsManager.fetchAll(SubscriptionManager);
        }
    }

    Component.onCompleted: {
        AlertsManager.fetchAll(SubscriptionManager);
    }
}
