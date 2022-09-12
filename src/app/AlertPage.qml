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
            Layout.fillWidth: true
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
            gesture.acceptedGestures: QtLocation.MapGestureArea.PinchGesture | QtLocation.MapGestureArea.PanGesture
            gesture.preventStealing: true

            Repeater {
                model: areaModel

                Repeater {
                    model: polygons

                    QtLocation.MapPolygon {
                        color: {
                            if (alert.msgType == AlertEntry.Cancel)
                                return Kirigami.Theme.disabledTextColor;
                            applicationWindow().severityTextColor(alertInfo.severity)
                        }
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
                        color: {
                            if (alert.msgType == AlertEntry.Cancel)
                                return Kirigami.Theme.disabledTextColor;
                            applicationWindow().severityTextColor(alertInfo.severity)
                        }
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
            // for diagnostics only
            QQC2.Label {
                Kirigami.FormData.label: i18n("Status:")
                text: {
                    switch(alert.status) {
                        case AlertEntry.Actual: return "Actual"
                        case AlertEntry.Exercise: return "Exercise"
                    }
                    return alert.status
                }
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Message type:")
                text: {
                    switch(alert.msgType) {
                        case AlertEntry.Alert: return "Alert"
                        case AlertEntry.Update: return "Update"
                        case AlertEntry.Cancel: return "Cancel"
                        case AlertEntry.Ack: return "Ack"
                        case AlertEntry.Error: return "Error"
                    }
                    return alert.msgType
                }
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Categories:")
                Layout.fillWidth: true
                text: CAPUtil.categoriesDisplayStrings(alertInfo.categories).join('\n')
                visible: alertInfo.categories != AlertInfo.Unknown
                wrapMode: Text.WordWrap
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
                Kirigami.FormData.label: i18n("Certainty:")
                text: CAPUtil.certaintyDisplayString(alertInfo.certainty)
                visible: alertInfo.certainty != AlertInfo.UnknownCertainty
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Recommended response:")
                Layout.fillWidth: true
                text: CAPUtil.responseTypesStrings(alertInfo.responseTypes).join('\n')
                visible: alertInfo.responseTypes != AlertInfo.UnknownResponseType
                wrapMode: Text.WordWrap
            }

            QQC2.Label {
                Kirigami.FormData.label: i18n("Web:")
                Layout.fillWidth: true
                text: alertInfo.web // TODO link
                visible: alertInfo.web
                wrapMode: Text.Wrap
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Contact:")
                Layout.fillWidth: true
                text: alertInfo.contact
                visible: alertInfo.contact
                wrapMode: Text.WordWrap
            }

            QQC2.Label {
                Kirigami.FormData.label: i18n("Sent:")
                visible: !isNaN(alert.sentTime.getTime())
                text: Qt.formatDateTime(alert.sentTime)
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Effective:")
                visible: !isNaN(alertInfo.effectiveTime.getTime())
                text: Qt.formatDateTime(alertInfo.effectiveTime)
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Onset:")
                visible: !isNaN(alertInfo.onsetTime.getTime())
                text: Qt.formatDateTime(alertInfo.onsetTime)
            }
            QQC2.Label {
                Kirigami.FormData.label: i18n("Expires:")
                visible: !isNaN(alertInfo.expireTime.getTime())
                text: Qt.formatDateTime(alertInfo.expireTime)
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
                    Layout.fillWidth: true
                    text:model.display
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
