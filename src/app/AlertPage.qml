/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtLocation as QtLocation
import QtPositioning
import org.kde.coreaddons
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.weathercore
import org.kde.publicalerts

FormCard.FormCardPage {
    id: root
    property capAlertMessage alert
    property capAlertInfo alertInfo
    property string sourceFile
    property url sourceUrl

    title: alertInfo.event

    data: [
        AreaModel {
            id: areaModel
            alertInfo: root.alertInfo
        }
    ]

    Kirigami.Heading {
        Layout.fillWidth: true
        text: root.alertInfo.headline
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
    }

    FormCard.FormCard {
        visible: infoSelector.count > 1
        FormCard.FormComboBoxDelegate {
            id: infoSelector
            text: i18n("Language")

            model: root.alert.alertInfos
            textRole: "languageDisplayName"
            currentIndex: root.alert.preferredInfoIndexForLocale

            onCurrentIndexChanged: root.alertInfo = root.alert.alertInfos[currentIndex];
        }
    }

    FormCard.FormHeader {
        title: i18n("Affected area")
        visible: map.visible
    }
    MapView {
        id: map
        visible: root.alertInfo.hasGeometry
        Layout.fillWidth: true
        Layout.preferredHeight: root.height / 2

        QtLocation.MapItemView {
            model: areaModel
            delegate: QtLocation.MapItemView {
                id: polygonsRoot
                required property capArea area
                required property var polygons
                model: polygons
                delegate: QtLocation.MapPolygon {
                    required property var modelData
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

                    HoverHandler { id: hover }
                    QQC2.ToolTip.text: polygonsRoot.area.description
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    // TODO this somehow positions the tooltips way off with Qt 6.9?
                    //QQC2.ToolTip.visible: hover.hovered
                }
            }
        }
        QtLocation.MapItemView {
            model: areaModel
            delegate: QtLocation.MapItemView {
                required property var circles
                model: circles
                delegate: QtLocation.MapCircle {
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

    FormCard.FormHeader {
        title: i18n("Event")
    }
    FormCard.FormCard {
        FormCard.FormTextDelegate {
            id: categoryDelegate
            text: i18nc("@label", "Categories")
            description: CAPUtil.categoriesDisplayStrings(root.alertInfo.categories).join('\n')
            visible: root.alertInfo.categories != CAPAlertInfo.Unknown
        }
        FormCard.FormDelegateSeparator {
            visible: categoryDelegate.visible
        }
        FormCard.FormTextDelegate {
            id: urgencyDelegate
            text: i18nc("@label", "Urgency")
            description: CAPUtil.urgencyDisplayString(root.alertInfo.urgency)
            visible: root.alertInfo.urgency != CAPAlertInfo.UnknownUrgency
        }
        FormCard.FormDelegateSeparator {
            visible: urgencyDelegate.visible
        }
        FormCard.FormTextDelegate {
            id: severityDelegate
            text: i18nc("@label", "Severity")
            description: root.alertInfo.severity !== CAPAlertInfo.UnknownSeverity ? CAPUtil.severityDisplayString(root.alertInfo.severity) : ""
            visible: root.alertInfo.severity !== CAPAlertInfo.UnknownSeverity
        }
        FormCard.FormDelegateSeparator {
            visible: severityDelegate.visible
        }
        FormCard.FormTextDelegate {
            id: certaintyDelegate
            text: i18nc("@label", "Certainty")
            description: CAPUtil.certaintyDisplayString(root.alertInfo.certainty)
            visible: root.alertInfo.certainty != CAPAlertInfo.UnknownCertainty
        }
        FormCard.FormDelegateSeparator {
            visible: certaintyDelegate.visible
        }
        FormCard.FormTextDelegate {
            id: onsetDelegate
            text: i18nc("@label, time", "Onset")
            description: Qt.formatDateTime(root.alertInfo.onsetTime)
            visible: !isNaN(root.alertInfo.onsetTime.getTime())
        }
        FormCard.FormDelegateSeparator {
            visible: onsetDelegate.visible
        }
        FormCard.FormTextDelegate {
            text: i18nc("@label", "Description")
            description: root.alertInfo.description
        }
    }

    FormCard.FormHeader {
        title: i18n("Act")
        visible: responseCard.visible
    }
    FormCard.FormCard {
        id: responseCard
        visible: responseDelegate.visible || instructionDelegate.visible

        FormCard.FormTextDelegate {
            id: responseDelegate
            text: i18nc("@label", "Recommended response")
            description: CAPUtil.responseTypesStrings(root.alertInfo.responseTypes).join('\n')
            visible: root.alertInfo.responseTypes != CAPAlertInfo.UnknownResponseType
        }
        FormCard.FormDelegateSeparator {
            visible: responseDelegate.visible
        }
        FormCard.FormTextDelegate {
            id: instructionDelegate
            text: i18nc("@label", "Instruction")
            description: root.alertInfo.instruction
            visible: description !== ""
        }
    }

    FormCard.FormHeader {
        title: i18n("Further Information")
        visible: contactCard.visible
    }
    FormCard.FormCard {
        id: contactCard
        visible: webDelegate.visible || contactDelegate.visible || senderDelegate.visible

        // TODO make these buttons if we got a valid url/phone number/email address/etc
        FormCard.FormTextDelegate {
            id: webDelegate
            text: i18nc("@label", "Web")
            description: root.alertInfo.web
            visible: root.alertInfo.web
            onLinkActivated: (link) => { Qt.openUrlExternally(link); }
        }
        FormCard.FormDelegateSeparator {
            visible: webDelegate.visible
        }
        FormCard.FormTextDelegate {
            id: contactDelegate
            text: i18nc("@label", "Contact")
            description: root.alertInfo.contact
            visible: root.alertInfo.contact
        }
        FormCard.FormDelegateSeparator {
            visible: contactDelegate.visible
        }
        FormCard.FormTextDelegate {
            id: senderDelegate
            text: i18nc("@label", "Sender")
            description: root.alertInfo.sender
            visible: text !== ""
        }
    }

    // TODO maybe move into event section and collapse when too long?
    // also: only show for aereas without geometry?
    FormCard.FormHeader {
        title: i18nc("@label", "Area")
        visible: areaCard.visible
    }
    FormCard.FormCard {
        id: areaCard
        visible: areaDelegate.description !== ""

        FormCard.FormTextDelegate {
            id: areaDelegate
            text: i18nc("@label", "Area")
            description: areaModel.allAreaDescriptions
            // visible: description !== ""
        }
    }

    FormCard.FormHeader {
        title: i18nc("@label", "Additional Resources")
        visible: resourceCard.visible
    }
    FormCard.FormCard {
        id: resourceCard
        visible: resourcesRepeater.count > 0

        Repeater {
            id: resourcesRepeater
            model: root.alertInfo.resources
            delegate: FormCard.FormButtonDelegate {
                id: delegateRoot
                required property capResource modelData

                text: delegateRoot.modelData.description
                icon.name: delegateRoot.modelData.mimeType.iconName
                description: delegateRoot.modelData.hasSize
                    ? i18n("%1 (%2)", delegateRoot.modelData.mimeType.comment, Format.formatByteSize(delegateRoot.modelData.size))
                    : delegateRoot.modelData.mimeType.comment
                onClicked: Qt.openUrlExternally(delegateRoot.modelData.uri)

                // TODO custom delegate for inline display of image content
                /* Image {
                    fillMode: Image.PreserveAspectFit
                    Layout.maximumWidth: implicitWidth
                    Layout.fillWidth: true
                    source: delegateRoot.modelData.mimeTypeName === "image/png" || delegateRoot.modelData.mimeTypeName === "image/jpeg" ? delegateRoot.modelData.uri : ""
                }*/
            }
        }
    }

    FormCard.FormHeader {
        title: i18n("Technical Information")
    }
    FormCard.FormCard {
        FormCard.FormTextDelegate {
            text: i18nc("@label", "Status")
            description: {
                switch(root.alert.status) {
                    case CAPAlertMessage.Actual: return "Actual";
                    case CAPAlertMessage.Exercise: return "Exercise"; // TODO we probably want some kind of exercise indicator instead?
                }
                return root.alert.status
            }
        }
        FormCard.FormDelegateSeparator {}
        FormCard.FormTextDelegate {
            text: i18nc("@label", "Message type")
            description: {
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
        FormCard.FormDelegateSeparator {}
        FormCard.FormTextDelegate {
            id: audienceDelegate
            text: i18n("Audience")
            description: root.alertInfo.audience
            visible: description !== ""
        }
        FormCard.FormDelegateSeparator {
            visible: audienceDelegate.visible
        }
        FormCard.FormTextDelegate {
            id: sentDelegate
            text: i18nc("@label, time", "Sent")
            description: Qt.formatDateTime(root.alert.sentTime)
            visible: !isNaN(root.alert.sentTime.getTime())
        }
        FormCard.FormDelegateSeparator {
            visible: sentDelegate.visible
        }
        FormCard.FormTextDelegate {
            id: effectiveDelegate
            text: i18nc("@label, time", "Effective")
            description: Qt.formatDateTime(root.alertInfo.effectiveTime)
            visible: !isNaN(root.alertInfo.effectiveTime.getTime())
        }
        FormCard.FormDelegateSeparator {
            visible: effectiveDelegate.visible
        }
        FormCard.FormTextDelegate {
            id: expireDelegate
            text: i18nc("@label, time", "Expires")
            description: Qt.formatDateTime(root.alertInfo.expireTime)
            visible: !isNaN(root.alertInfo.expireTime.getTime())
        }
        FormCard.FormDelegateSeparator {
            visible: expireDelegate.visible
        }
        FormCard.FormTextDelegate {
            text: i18n("Identifier")
            description: root.alert.identifier
            // TODO make copyable
        }
        FormCard.FormDelegateSeparator {}
        FormCard.FormButtonDelegate {
            text: i18n("View Source File")
            icon.name: "document-open-symbolic"
            onClicked: Qt.openUrlExternally("file://" + root.sourceFile)
        }
        FormCard.FormDelegateSeparator {}
        FormCard.FormButtonDelegate {
            text: i18n("Open Source File")
            icon.name: "internet-services-symbolic"
            onClicked: Qt.openUrlExternally(root.sourceUrl)
            // TODO copying this would be more useful than opening it directly
        }
        // TODO add source CAP links (once we have those in CAP as custom event codes)
    }
}
