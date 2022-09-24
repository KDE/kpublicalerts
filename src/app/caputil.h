/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_CAPUTIL_H
#define KPUBLICALERTS_CAPUTIL_H

#include <KWeatherCore/CAPAlertInfo>

#include <QColor>

namespace KPublicAlerts {

/** Utility functions for using CAP data from QML. */
class CAPUtil
{
    Q_GADGET
public:
    Q_INVOKABLE static QString severityDisplayString(KWeatherCore::CAPAlertInfo::Severity severity);
    Q_INVOKABLE static QString urgencyDisplayString(KWeatherCore::CAPAlertInfo::Urgency urgency);
    Q_INVOKABLE static QString certaintyDisplayString(KWeatherCore::CAPAlertInfo::Certainty certainty);
    Q_INVOKABLE static QStringList categoriesDisplayStrings(KWeatherCore::CAPAlertInfo::Categories categories);
    Q_INVOKABLE static QStringList categoriesDisplayStrings(uint categories);
    Q_INVOKABLE static QStringList responseTypesStrings(KWeatherCore::CAPAlertInfo::ResponseTypes responseTypes);
    Q_INVOKABLE static QStringList responseTypesStrings(uint responseTypes);

    Q_INVOKABLE static QString categoriesIconName(KWeatherCore::CAPAlertInfo::Categories categories);
    Q_INVOKABLE static QString categoriesIconName(uint categories);

    Q_INVOKABLE static QColor colorMix(const QColor &c1, const QColor &c2, double bias = 0.5);
};

}

#endif // KPUBLICALERTS_CAPUTIL_H
