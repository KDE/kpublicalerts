/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtLocation 5.11 as QtLocation
import QtPositioning 5.11
import org.kde.kirigami 2.15 as Kirigami

import org.kde.publicalerts 1.0

Kirigami.Page {
    id: root
    title: i18n("Add area of interest")

   actions.main: Kirigami.Action {
        icon.name: "list-add"
        text: i18n("Add area of interest")
        enabled: nameField.text != ""
        onTriggered: {
            SubscriptionManager.addSubscription(map.center.latitude, map.center.longitude, 20000, nameField.text);
            applicationWindow().pageStack.pop();
        }
    }

    QtLocation.Plugin {
        id: mapPlugin
        required.mapping: QtLocation.Plugin.AnyMappingFeatures
        preferred: ["osm"]
    }

    QtLocation.Map {
        id: map
        Layout.fillWidth: true
        Layout.preferredHeight: root.height / 2
        plugin: mapPlugin
        anchors.fill: parent
        gesture.acceptedGestures: QtLocation.MapGestureArea.PinchGesture | QtLocation.MapGestureArea.PanGesture
        gesture.preventStealing: true

        QtLocation.MapCircle {
            color: Kirigami.Theme.highlightColor
            center: map.center
            opacity: 0.25
            border.color: color
            border.width: 2
            radius: 20 * 1000.0
        }

        Component.onCompleted: fitViewportToVisibleMapItems()
    }

    footer: Kirigami.FormLayout {
        QQC2.TextField {
            id: nameField
            Kirigami.FormData.label: i18n("Name:")
        }
    }
}

