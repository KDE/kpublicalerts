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

import org.kde.weathercore
import org.kde.publicalerts

Kirigami.ScrollablePage {
    id: root
    property capAlertMessage alert
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
            text: root.alertInfo.headline
            wrapMode: Text.WordWrap
        }

        MapView {
            id: map
            Layout.fillWidth: true
            Layout.preferredHeight: root.height / 2

            Repeater {
                model: areaModel

                Repeater {
                    model: polygons

                    QtLocation.MapPolygon {
                        color: {
                            if (root.alert.messageType === CAPAlertMessage.Cancel || root.alertInfo.responseTypes === CAPAlertInfo.AllClear)
                                return Kirigami.Theme.disabledTextColor;
                            if (root.alertInfo.severity === CAPAlertInfo.UnknownSeverity)
                                return Kirigami.Theme.highlightColor;
                            applicationWindow().severityTextColor(root.alertInfo.severity)
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
                        required property capCircle modelData
                        color: {
                            if (root.alert.messageType === CAPAlertMessage.Cancel || root.alertInfo.responseTypes === CAPAlertInfo.AllClear)
                                return Kirigami.Theme.disabledTextColor;
                            if (root.alert.severity === CAPAlertInfo.UnknownSeverity)
                                return Kirigami.Theme.highlightColor;
                            applicationWindow().severityTextColor(root.alertInfo.severity)
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

            function centerOnArea() {
                const center = areaModel.center();
                map.center = QtPositioning.coordinate(center.y, center.x);
                map.zoomLevel = areaModel.zoomLevel(map.width, map.height);
            }

            onWidthChanged: centerOnArea();
            onHeightChanged: centerOnArea();
            Component.onCompleted: centerOnArea();
        }

        Kirigami.FormLayout {
            Layout.fillWidth: true
            // for diagnostics only
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label", "Status:")
                text: {
                    switch(root.alert.status) {
                        case CAPAlertMessage.Actual: return "Actual"
                        case CAPAlertMessage.Exercise: return "Exercise"
                    }
                    return root.alert.status
                }
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label", "Message type:")
                text: {
                    switch(root.alert.messageType) {
                        case CAPAlertMessage.Alert: return "Alert"
                        case CAPAlertMessage.Update: return "Update"
                        case CAPAlertMessage.Cancel: return "Cancel"
                        case CAPAlertMessage.Acknowledge: return "Acknowledge"
                        case CAPAlertMessage.Error: return "Error"
                    }
                    return root.alert.messageType
                }
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label", "Categories:")
                Layout.fillWidth: true
                text: CAPUtil.categoriesDisplayStrings(root.alertInfo.categories).join('\n')
                visible: root.alertInfo.categories != CAPAlertInfo.Unknown
                wrapMode: Text.WordWrap
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label", "Urgency:")
                text: CAPUtil.urgencyDisplayString(root.alertInfo.urgency)
                visible: root.alertInfo.urgency != CAPAlertInfo.UnknownUrgency
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label", "Severity:")
                text: root.alertInfo.severity !== CAPAlertInfo.UnknownSeverity ? CAPUtil.severityDisplayString(root.alertInfo.severity) : ""
                visible: root.alertInfo.severity !== CAPAlertInfo.UnknownSeverity
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label", "Certainty:")
                text: CAPUtil.certaintyDisplayString(root.alertInfo.certainty)
                visible: root.alertInfo.certainty != CAPAlertInfo.UnknownCertainty
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label", "Recommended response:")
                Layout.fillWidth: true
                text: CAPUtil.responseTypesStrings(root.alertInfo.responseTypes).join('\n')
                visible: root.alertInfo.responseTypes != CAPAlertInfo.UnknownResponseType
                wrapMode: Text.WordWrap
            }

            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label", "Web:")
                Layout.fillWidth: true
                text: root.alertInfo.web
                visible: root.alertInfo.web
                wrapMode: Text.Wrap
                onLinkActivated: Qt.openUrlExternally(link)
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label", "Contact:")
                Layout.fillWidth: true
                text: root.alertInfo.contact
                visible: root.alertInfo.contact
                wrapMode: Text.WordWrap
            }

            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label, time", "Sent:")
                visible: !isNaN(root.alert.sentTime.getTime())
                text: Qt.formatDateTime(root.alert.sentTime)
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label, time", "Effective:")
                visible: !isNaN(root.alertInfo.effectiveTime.getTime())
                text: Qt.formatDateTime(root.alertInfo.effectiveTime)
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label, time", "Onset:")
                visible: !isNaN(root.alertInfo.onsetTime.getTime())
                text: Qt.formatDateTime(root.alertInfo.onsetTime)
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label, time", "Expires:")
                visible: !isNaN(root.alertInfo.expireTime.getTime())
                text: Qt.formatDateTime(root.alertInfo.expireTime)
            }
            QQC2.Label {
                Kirigami.FormData.label: i18nc("@label", "Sender:")
                Layout.fillWidth: true
                text: root.alertInfo.sender
                wrapMode: Text.WordWrap
                visible: text !== ""
            }

            QQC2.Label {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18nc("@label", "Description")
                Kirigami.Separator {}
            }
            QQC2.Label {
                Kirigami.FormData.isSection: true
                text: root.alertInfo.description
                wrapMode: Text.WordWrap
            }

            QQC2.Label {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18nc("@label", "Instruction")
                Kirigami.Separator {}
                visible: instructionText.visible
            }
            QQC2.Label {
                id: instructionText
                Kirigami.FormData.isSection: true
                text: root.alertInfo.instruction
                wrapMode: Text.WordWrap
                visible: text !== ""
            }

            QQC2.Label {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18nc("@label", "Area")
                Kirigami.Separator {}
                visible: areaDescLabel.visible
            }
            QQC2.Label {
                id: areaDescLabel
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: areaModel.allAreaDescriptions
                visible: text !== ""
            }
        }
    }

}
