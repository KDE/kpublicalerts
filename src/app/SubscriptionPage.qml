/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtPositioning
import org.kde.kirigami as Kirigami

import org.kde.publicalerts

Kirigami.Page {
    id: root
    title: i18nc("@title:window", "Add Area of Interest")
    property var subscription

   actions: [
        Kirigami.Action {
            icon.name: "list-add"
            text: i18nc("@action:button", "Add Area of Interest")
            enabled: nameField.text !== "" && marker.topLeft.isValid
            onTriggered: {
                root.subscription.name = nameField.text;
                root.subscription.area = Qt.rect(marker.topLeft.longitude, marker.bottomRight.latitude, marker.bottomRight.longitude - marker.topLeft.longitude, marker.topLeft.latitude - marker.bottomRight.latitude);
                SubscriptionManager.add(root.subscription);
                applicationWindow().pageStack.pop();
            }
            visible: root.subscription.id === ""
        },
        Kirigami.Action {
            icon.name: "document-save"
            text: i18nc("@action:button", "Save")
            enabled: nameField.text !== ""
            onTriggered: {
                root.subscription.name = nameField.text;
                root.subscription.area = Qt.rect(marker.topLeft.longitude, marker.bottomRight.latitude, marker.bottomRight.longitude - marker.topLeft.longitude, marker.topLeft.latitude - marker.bottomRight.latitude);
                SubscriptionManager.update(root.subscription);
                applicationWindow().pageStack.pop();
            }
            visible: root.subscription.id !== ""
        }
    ]

    MapView {
        id: map
        Layout.fillWidth: true
        Layout.preferredHeight: root.height / 2
        anchors.fill: parent

        SubscriptionArea {
            id: marker
            topLeft: root.subscription.id !== "" ? QtPositioning.coordinate(root.subscription.area.top, root.subscription.area.left) : undefined
            bottomRight: root.subscription.id !== "" ? QtPositioning.coordinate(root.subscription.area.bottom, root.subscription.area.right) : undefined
            Component.onCompleted: marker.normalize()
        }

        TapHandler {
            acceptedButtons: Qt.LeftButton
            onTapped: (eventPoint) => {
                const isFirstClick = !marker.topLeft.isValid;
                marker.setCenter(map.toCoordinate(eventPoint.position));
                if (isFirstClick)
                    map.fitViewportToVisibleMapItems();
            }
        }
    }

    footer: Kirigami.FormLayout {
        QQC2.TextField {
            id: nameField
            Kirigami.FormData.label: i18nc("@label", "Name:")
            text: root.subscription.name
        }
    }

    Timer {
        id: editPositionTimer
        repeat: false
        interval: 100
        onTriggered: {
            map.fitViewportToVisibleMapItems()
            map.zoomLevel = map.zoomLevel - 1;
        }
    }

    Component.onCompleted: editPositionTimer.start()
}

