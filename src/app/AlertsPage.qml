/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami

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
                // TODO AlertInfo  enum properties are not accessible
                return "dialog-warning"
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
