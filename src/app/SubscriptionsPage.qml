/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami

import org.kde.publicalerts 1.0

Kirigami.ScrollablePage {
    id: root
    title: i18n("Areas of interest")

    Component {
        id: subscriptionPage
        SubscriptionPage {}
    }

    actions.main: Kirigami.Action {
        icon.name: "list-add"
        text: i18n("Add...")
        onTriggered: applicationWindow().pageStack.push(subscriptionPage)
    }

    ListView {
        id: listView
        model: SubscriptionManager
        delegate: Kirigami.SwipeListItem {
            readonly property var sourceModel: ListView.view.model
            QQC2.Label {
                text: model.display
            }
            actions: [
                Kirigami.Action {
                    iconName: "edit-delete"
                    text: i18n("Unsubscribe")
                    onTriggered: {
                        sourceModel.removeRows(model.index, 1)
                    }
                }
            ]
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: listView.count == 0
            text: i18n("No areas of interest chosen.")
            helpfulAction: Kirigami.Action {
                icon.name: "list-add"
                text: i18n("Add...")
                onTriggered: applicationWindow().pageStack.push(subscriptionPage)
            }
        }
    }
}
