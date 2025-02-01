/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

import org.kde.publicalerts

Kirigami.ScrollablePage {
    id: root
    title: i18nc("@title:window", "Areas of Interest")

    Component {
        id: subscriptionPage
        SubscriptionPage {}
    }

    actions: [
        Kirigami.Action {
            icon.name: "list-add"
            text: i18nc("@action:button", "Add…")
            onTriggered: applicationWindow().pageStack.push(subscriptionPage)
        }
    ]

    ListView {
        id: listView
        model: SubscriptionManager
        delegate: Kirigami.SwipeListItem {
            readonly property var sourceModel: ListView.view.model
            contentItem: QQC2.Label {
                text: model.display
            }
            actions: [
                Kirigami.Action {
                    icon.name: "edit-delete"
                    text: i18nc("@action:button", "Unsubscribe")
                    onTriggered: {
                        sourceModel.removeRows(model.index, 1)
                    }
                }
            ]
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: listView.count == 0
            text: i18nc("@info:placeholder", "No areas of interest chosen")
            helpfulAction: Kirigami.Action {
                icon.name: "list-add"
                text: i18nc("@action:button", "Add…")
                onTriggered: applicationWindow().pageStack.push(subscriptionPage)
            }
        }
    }
}
