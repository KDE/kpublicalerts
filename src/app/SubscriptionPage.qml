/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtLocation as QtLocation
import QtPositioning
import org.kde.kirigami as Kirigami

import org.kde.publicalerts

Kirigami.Page {
    id: root
    title: i18nc("@title:window", "Add Area of Interest")

   actions: [
        Kirigami.Action {
            icon.name: "list-add"
            text: i18nc("@action:button", "Add Area of Interest")
            enabled: nameField.text != ""
            onTriggered: {
                SubscriptionManager.addSubscription(map.center.latitude, map.center.longitude, radiusSlider.value, nameField.text);
                applicationWindow().pageStack.pop();
            }
        }
    ]

    MapView {
        id: map
        Layout.fillWidth: true
        Layout.preferredHeight: root.height / 2
        anchors.fill: parent

        QtLocation.MapCircle {
            color: Kirigami.Theme.highlightColor
            center: map.center
            opacity: 0.25
            border.color: color
            border.width: 2
            radius: radiusSlider.value
        }

        Component.onCompleted: fitViewportToVisibleMapItems()
    }

    footer: Kirigami.FormLayout {
        QQC2.TextField {
            id: nameField
            Kirigami.FormData.label: i18nc("@label", "Name:")
        }
        RowLayout {
            Kirigami.FormData.label: i18nc("@label", "Radius (km)")
            QQC2.Slider {
                id: radiusSlider
                from: 1000
                to: 50000
                value: 20000
            }
            QQC2.Label {
                text: i18nc("@label", "%1 km", radiusSlider.value / 1000.0)
            }
        }
    }
}

