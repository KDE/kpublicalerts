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

import org.kde.weathercore 1.0
import org.kde.publicalerts 1.0

Kirigami.ScrollablePage {
    id: root
    property var alert
    property alias alertInfo: areaModel.alertInfo

    title: alertInfo.event

    AreaModel {
        id: areaModel
    }

    ColumnLayout {
        width: parent.width
        QQC2.Label {
            Kirigami.FormData.isSection: true
            text: alertInfo.headline
            wrapMode: Text.WordWrap
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

            Repeater {
                model: areaModel

                Repeater {
                    model: polygons

                    QtLocation.MapPolygon {
                        color: Kirigami.Theme.negativeTextColor
                        opacity: 0.25
                        border.color: color
                        border.width: 2
                        path: modelData
                        Component.onCompleted: console.log(path, model.polygons, model.polygons.path)
                    }
                }
            }

            Component.onCompleted: fitViewportToVisibleMapItems()
        }

        Kirigami.FormLayout {
            Layout.fillWidth: true
            QQC2.Label {
                Kirigami.FormData.label: i18n("Status:")
                text: alert.status
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Categories:")
                text: alertInfo.categories // TODO
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Urgency:")
                text: alertInfo.urgency // TODO
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Severity:")
                text: alertInfo.severity // TODO
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Effective:")
                text: alertInfo.effectiveTime // TODO
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Onset:")
                text: alertInfo.onsetTime // TODO
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Expires:")
                text: alertInfo.expireTime // TODO
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Sender:")
                text: alertInfo.sender
            }

            QQC2.Label {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18n("Description")
                Kirigami.Separator {}
            }
            QQC2.Label {
                Kirigami.FormData.isSection: true
                text: alertInfo.description
                wrapMode: Text.WordWrap
            }

            QQC2.Label {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18n("Instruction")
                Kirigami.Separator {}
            }
            QQC2.Label {
                Kirigami.FormData.isSection: true
                text: alertInfo.instruction
                wrapMode: Text.WordWrap
            }

            QQC2.Label {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18n("Area")
                Kirigami.Separator {}
            }
            Repeater {
                model: areaModel
                QQC2.Label {
                    text:model.display
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
