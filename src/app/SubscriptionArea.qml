/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtLocation as QtLocation
import QtPositioning as QtPositioning
import org.kde.kirigami as Kirigami

QtLocation.MapItemGroup {
    id: root

    property alias topLeft: background.topLeft
    property alias bottomRight: background.bottomRight

    QtLocation.MapRectangle {
        id: background
        color: Kirigami.Theme.highlightColor
        opacity: 0.25
        border.color: color
        border.width: 2
        autoFadeIn: false
        referenceSurface: QtLocation.QtLocation.ReferenceSurface.Globe

        DragHandler { cursorShape: Qt.ClosedHandCursor }
        HoverHandler { cursorShape: Qt.OpenHandCursor }
    }

    function setCenter(pos) {
        if (!background.topLeft.isValid) {
            background.topLeft = QtPositioning.QtPositioning.coordinate(0.1, -0.1);
        }
        if (!background.bottomRight.isValid) {
            background.bottomRight = QtPositioning.QtPositioning.coordinate(-0.1, 0.1);
        }

        const lx = Math.abs(background.bottomRight.longitude - background.topLeft.longitude) / 2;
        const ly = Math.abs(background.bottomRight.latitude - background.topLeft.latitude) / 2;

        background.topLeft = QtPositioning.QtPositioning.coordinate(pos.latitude + ly, pos.longitude - lx);
        background.bottomRight = QtPositioning.QtPositioning.coordinate(pos.latitude - ly, pos.longitude + lx);
    }

    // without coordinates in the right order drag bindings from the background to the handles don't work
    function normalize() {
        const lon1 = background.topLeft.longitude;
        const lon2 = background.bottomRight.longitude;
        const lat1 = background.topLeft.latitude;
        const lat2 = background.bottomRight.latitude;

        background.topLeft = QtPositioning.QtPositioning.coordinate(Math.max(lat1, lat2), Math.min(lon1, lon2));
        background.bottomRight = QtPositioning.QtPositioning.coordinate(Math.min(lat1, lat2), Math.max(lon1, lon2));
    }

    component Handle : QtLocation.MapQuickItem {
        id: handleRoot
        property alias cursorShape: hoverHandler.cursorShape
        signal dragFinished()

        anchorPoint { x: Kirigami.Units.largeSpacing; y: Kirigami.Units.largeSpacing }
        autoFadeIn: false
        sourceItem: Rectangle {
            height: Kirigami.Units.largeSpacing * 2
            width: height
            color: Kirigami.Theme.highlightColor
            radius: height / 2
        }
        DragHandler {
            onActiveChanged: {
                if (!active)
                    handleRoot.dragFinished()
            }
        }
        HoverHandler {
            id: hoverHandler
        }
    }

    Handle {
        id: topLeftHandle
        coordinate: background.topLeft
        cursorShape: Qt.SizeFDiagCursor
        onCoordinateChanged: background.topLeft = coordinate
        onDragFinished: root.normalize()
    }

    Handle {
        id: topRightHandle
        coordinate: QtPositioning.QtPositioning.coordinate(background.topLeft.latitude, background.bottomRight.longitude)
        cursorShape: Qt.SizeBDiagCursor
        onCoordinateChanged: {
            background.topLeft.latitude = coordinate.latitude;
            background.bottomRight.longitude = coordinate.longitude;
        }
        onDragFinished: root.normalize()
    }

    Handle {
        id: bottomLeftHandle
        coordinate: QtPositioning.QtPositioning.coordinate(background.bottomRight.latitude, background.topLeft.longitude)
        cursorShape: Qt.SizeBDiagCursor
        onCoordinateChanged: {
            background.topLeft.longitude = coordinate.longitude;
            background.bottomRight.latitude = coordinate.latitude;
        }
        onDragFinished: root.normalize()
    }

    Handle {
        id: bottomRightHandle
        coordinate: background.bottomRight
        cursorShape: Qt.SizeFDiagCursor
        onCoordinateChanged: background.bottomRight = coordinate
        onDragFinished: root.normalize()
    }
}
