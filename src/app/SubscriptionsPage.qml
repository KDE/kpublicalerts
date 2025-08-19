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
            onTriggered: applicationWindow().pageStack.push(subscriptionPage, { subscription: SubscriptionManager.makeSubscription() })
        }
    ]

    ListView {
        id: listView
        model: SubscriptionManager
        delegate: Kirigami.SwipeListItem {
            id: delegateRoot
            required property var subscription
            required property string name
            required property bool isSubscribed
            required property bool awaitsConfirmation
            required property int index

            readonly property var sourceModel: ListView.view.model
            contentItem: Kirigami.IconTitleSubtitle {
                icon.name: {
                    if (!delegateRoot.isSubscribed)
                        return "dialog-error";
                    return delegateRoot.awaitsConfirmation ? "dialog-ok" : "dialog-positive";
                }
                title: delegateRoot.name
            }
            actions: [
                Kirigami.Action {
                    icon.name: "edit-delete"
                    text: i18nc("@action:button", "Unsubscribe")
                    onTriggered: {
                        delegateRoot.sourceModel.removeRows(delegateRoot.index, 1)
                    }
                }
            ]
            onClicked: applicationWindow().pageStack.push(subscriptionPage, { subscription: delegateRoot.subscription })
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
