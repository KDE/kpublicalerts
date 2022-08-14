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
                        color: applicationWindow().severityTextColor(alertInfo.severity)
                        opacity: 0.25
                        border.color: color
                        border.width: 2
                        path: modelData
                    }
                }
            }
            Repeater {
                model: areaModel
                Repeater {
                    model: circles

                    QtLocation.MapCircle {
                        color: applicationWindow().severityTextColor(alertInfo.severity)
                        opacity: 0.25
                        border.color: color
                        border.width: 2
                        center {
                            latitude: modelData.latitude
                            longitude: modelData.longitude
                        }
                        // needed in meters here, provided in kilometers
                        radius: Math.max(1.0, modelData.radius) * 1000.0
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
                text: CAPUtil.categoriesDisplayStrings(alertInfo.categories).join('\n')
                visible: alertInfo.categories != AlertInfo.Unknown
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Urgency:")
                text: CAPUtil.urgencyDisplayString(alertInfo.urgency)
                visible: alertInfo.urgency != AlertInfo.UnknownUrgency
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Severity:")
                text: CAPUtil.severityDisplayString(alertInfo.severity)
                visible: alertInfo.severity != AlertInfo.UnknownSeverity
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Recommended response:")
                text: CAPUtil.responseTypesStrings(alertInfo.responseTypes).join('\n')
                visible: alertInfo.responseTypes != AlertInfo.UnknownResponseType
            }

            QQC2.Label {
                Kirigami.FormData.label: i18n("Web:")
                text: alertInfo.web // TODO link
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Contact:")
                text: alertInfo.contact
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
