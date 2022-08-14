/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_CAPUTIL_H
#define KPUBLICALERTS_CAPUTIL_H

#include <KWeatherCore/AlertInfo>

namespace KPublicAlerts {

/** Utility functions for using CAP data from QML. */
class CAPUtil
{
    Q_GADGET
public:
    Q_INVOKABLE static QString severityDisplayString(KWeatherCore::AlertInfo::Severity severity);
    Q_INVOKABLE static QString urgencyDisplayString(KWeatherCore::AlertInfo::Urgency urgency);
    Q_INVOKABLE static QStringList categoriesDisplayStrings(KWeatherCore::AlertInfo::Categories categories);
    Q_INVOKABLE static QStringList categoriesDisplayStrings(uint categories);
    Q_INVOKABLE static QStringList responseTypesStrings(KWeatherCore::AlertInfo::ResponseTypes responseTypes);
    Q_INVOKABLE static QStringList responseTypesStrings(uint responseTypes);
};

}

#endif // KPUBLICALERTS_CAPUTIL_H
