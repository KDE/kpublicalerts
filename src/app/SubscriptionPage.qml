/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtPositioning
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiAddons

import org.kde.publicalerts

Kirigami.Page {
    id: root
    title: i18nc("@title:window", "Add Area of Interest")
    property var subscription
    padding: 0

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

    KirigamiAddons.FloatingToolBar {
        anchors {
            bottom: parent.bottom
            margins: Kirigami.Units.largeSpacing
            horizontalCenter: parent.horizontalCenter
        }

        contentItem: RowLayout {
            QQC2.TextField {
                id: nameField
                placeholderText: i18n("Name")
                text: root.subscription.name
            }
            QQC2.ToolButton {
                icon.name: root.subscription.id === "" ? "list-add" : "document-save"
                text: root.subscription.id === "" ? i18nc("@action:button", "Add") : i18nc("@action:button", "Save")
                enabled: nameField.text !== "" && marker.topLeft.isValid
                onClicked: {
                    root.subscription.name = nameField.text;
                    root.subscription.area = Qt.rect(marker.topLeft.longitude, marker.bottomRight.latitude, marker.bottomRight.longitude - marker.topLeft.longitude, marker.topLeft.latitude - marker.bottomRight.latitude);
                    if (root.subscription.id === "") {
                        SubscriptionManager.add(root.subscription);
                    } else {
                        SubscriptionManager.update(root.subscription);
                    }
                    QQC2.ApplicationWindow.window.pageStack.layers.pop();
                }
            }
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

