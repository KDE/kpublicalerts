/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami

import org.kde.weathercore 1.0
import org.kde.publicalerts 1.0

Kirigami.ScrollablePage {
    id: root
    title: i18n("Current Alerts")

    ListView {
        id: listView
        model: AlertsManager
        delegate: Kirigami.BasicListItem {
            text: model.alertInfo.headline
            subtitle: model.alertInfo.description
            icon: {
                if (model.alert.msgType == AlertEntry.Cancel)
                    return "dialog-ok";
                if (model.alertInfo.categories & AlertInfo.Fire)
                    return "hotspot";
                if (model.alertInfo.categories & AlertInfo.Met)
                    return "weather-storm";
                // TODO AlertInfo  enum properties are not accessible
                return "dialog-warning";
            }
            iconColor: {
                if (model.alert.msgType == AlertEntry.Cancel)
                    return Kirigami.Theme.positiveTextColor;
                console.log(model.alertInfo.severity)
                if (model.alertInfo.severity == AlertInfo.Extreme || model.alertInfo.severity == AlertInfo.Severe)
                    return Kirigami.Theme.negativeTextColor;
                return Kirigami.Theme.neutralTextColor;
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: listView.count == 0
            text: "No alerts"
        }
    }

    // TODO for testing
    Component.onCompleted: {
        AlertsManager.fetchAll();
    }
}
