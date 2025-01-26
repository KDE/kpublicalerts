/*
    SPDX-FileCopyrightText: 2019-2023 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

pragma Singleton

import QtQuick
import QtLocation as QtLocation

QtObject {
    property QtLocation.Plugin plugin: QtLocation.Plugin {
        name: "osm"
        QtLocation.PluginParameter {
            name: "osm.useragent"
            value: "org.kde.publicalerts/" + Application.version
        }
        QtLocation.PluginParameter {
            name: "osm.mapping.providersrepository.address"
            value: "https://autoconfig.kde.org/qtlocation/"
        }
    }
}
