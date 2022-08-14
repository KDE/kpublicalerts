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

Kirigami.ApplicationWindow {
    id: root

    title: i18n("Public Alerts")

    // should be in CAPUtil in C++, but that would require KF5::ConfigWidgets for the color scheme...
    function severityTextColor(severity)
    {
        switch(severity) {
            case AlertInfo.Extreme: return Kirigami.Theme.negativeTextColor;
            case AlertInfo.Severe: return Kirigami.Theme.neutralTextColor;
            case AlertInfo.Moderate: return CAPUtil.colorMix(Kirigami.Theme.neutralTextColor, Kirigami.Theme.positiveTextColor, 0.35);
            case AlertInfo.Minor: return Kirigami.Theme.positiveTextColor;
        }
        return Kirigami.Theme.textColor;
    }

    pageStack.initialPage: AlertsPage {}
}
