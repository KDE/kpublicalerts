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
    property var alert
    property var alertInfo

    title: alertInfo.event

    Kirigami.FormLayout {
        width: parent.width
        QQC2.Label {
            Kirigami.FormData.isSection: true
            text: alertInfo.headline
            wrapMode: Text.WordWrap
        }

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
    }
}
