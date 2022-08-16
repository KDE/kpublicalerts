/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "caputil.h"

#include <KLocalizedString>
#include <KLazyLocalizedString>

#include <KColorUtils>

#include <QDebug>
#include <QLocale>

#include <bit>

using namespace KPublicAlerts;

template<typename T>
struct MapEntry {
    T value;
    KLazyLocalizedString name;
};

template<typename EnumT, std::size_t N>
static QString valueToString(EnumT value, const MapEntry<EnumT> (&map)[N])
{
    const auto it = std::find_if(std::begin(map), std::end(map), [value](const auto &entry) { return entry.value == value; });
    if (it != std::end(map)) {
        return (*it).name.toString();
    }
    qWarning() << "got unknown enum value:" << (int)value;
    return QString::number((int)value);
}

template <typename FlagT, typename EnumT, std::size_t N>
static QStringList flagToString(FlagT value, const MapEntry<EnumT> (&map)[N])
{
    QStringList l;
    l.reserve(std::popcount((uint)value));
    for (const auto &entry : map) {
        if ((uint)entry.value & (uint)value) {
            l.push_back(entry.name.toString());
        }
    }
    return l;
}

// see https://docs.oasis-open.org/emergency/cap/v1.2/CAP-v1.2.html
static constexpr const MapEntry<KWeatherCore::AlertInfo::Severity> severity_map[] = {
    { KWeatherCore::AlertInfo::Severity::Extreme, kli18n("Extreme - Extraordinary threat to life or property") },
    { KWeatherCore::AlertInfo::Severity::Severe, kli18n("Severe - Significant threat to life or property") },
    { KWeatherCore::AlertInfo::Severity::Moderate, kli18n("Moderate - Possible threat to life or property") },
    { KWeatherCore::AlertInfo::Severity::Minor, kli18n("Minor - Minimal to no known threat to life or property") },
};

static constexpr const MapEntry<KWeatherCore::AlertInfo::Urgency> urgency_map[] = {
    { KWeatherCore::AlertInfo::Urgency::Immediate, kli18n("Responsive action should be taken immediately") },
    { KWeatherCore::AlertInfo::Urgency::Expected, kli18n("Responsive action should be taken soon") },
    { KWeatherCore::AlertInfo::Urgency::Future, kli18n("Responsive action be taken in the near future") },
    { KWeatherCore::AlertInfo::Urgency::Past, kli18n("Responsive action is no longer required") },
};

static constexpr const MapEntry<KWeatherCore::AlertInfo::Certainty> certainty_map[] = {
    { KWeatherCore::AlertInfo::Certainty::Observed, kli18n("Occurred or ongoing") },
    { KWeatherCore::AlertInfo::Certainty::Likely, kli18n("Likely (>50%)") },
    { KWeatherCore::AlertInfo::Certainty::Possible, kli18n("Possible but not likely (<50%)") },
    { KWeatherCore::AlertInfo::Certainty::Unlikely, kli18n("Not expected to occur") },
};

static constexpr const MapEntry<KWeatherCore::AlertInfo::Category> category_map[] = {
    { KWeatherCore::AlertInfo::Category::Geo, kli18n("Geophysical") },
    { KWeatherCore::AlertInfo::Category::Met, kli18n("Meteorological") },
    { KWeatherCore::AlertInfo::Category::Safety, kli18n("General emergency and public safety") },
    { KWeatherCore::AlertInfo::Category::Security, kli18n("Law enforcement, military, homeland and local/private security") },
    { KWeatherCore::AlertInfo::Category::Rescue, kli18n("Rescue and recovery") },
    { KWeatherCore::AlertInfo::Category::Fire, kli18n("Fire") },
    { KWeatherCore::AlertInfo::Category::Health, kli18n("Medical and public health") },
    { KWeatherCore::AlertInfo::Category::Env, kli18n("Pollution and other environmental event") },
    { KWeatherCore::AlertInfo::Category::Transport, kli18n("Transportation") },
    { KWeatherCore::AlertInfo::Category::Infra, kli18n("Utility, telecommunication, other non-transport infrastructure") },
    { KWeatherCore::AlertInfo::Category::CBRNE, kli18n("Chemical, Biological, Radiological, Nuclear or High-Yield Explosive threat or attack") },
    { KWeatherCore::AlertInfo::Category::Other, kli18n("Other event") },
};

static constexpr const MapEntry<KWeatherCore::AlertInfo::ResponseType> response_map[] = {
    { KWeatherCore::AlertInfo::ResponseType::Shelter, kli18n("Take shelter in place or per instructions.") },
    { KWeatherCore::AlertInfo::ResponseType::Evacuate, kli18n("Relocate as instructed.") },
    { KWeatherCore::AlertInfo::ResponseType::Prepare, kli18n("Make preparations as per the instructions.") },
    { KWeatherCore::AlertInfo::ResponseType::Execute, kli18n("Execute activities identified in the instructions.") },
    { KWeatherCore::AlertInfo::ResponseType::Avoid, kli18n("Avoid the subject event as per the instructions.") },
    { KWeatherCore::AlertInfo::ResponseType::Monitor, kli18n("Attend to information sources as described in the instructions.") },
    { KWeatherCore::AlertInfo::ResponseType::Assess, kli18n("Evaluate the information in this message.") },
    { KWeatherCore::AlertInfo::ResponseType::AllClear, kli18n("The subject event no longer poses a threat or concern and any follow on action is described in the instructions.") },
    { KWeatherCore::AlertInfo::ResponseType::None, kli18n("No action recommended.") },
};

QString CAPUtil::severityDisplayString(KWeatherCore::AlertInfo::Severity severity)
{
    return valueToString(severity, severity_map);
}

QString CAPUtil::urgencyDisplayString(KWeatherCore::AlertInfo::Urgency urgency)
{
    return valueToString(urgency, urgency_map);
}

QString CAPUtil::certaintyDisplayString(KWeatherCore::AlertInfo::Certainty certainty)
{
    return valueToString(certainty, certainty_map);
}

QStringList CAPUtil::categoriesDisplayStrings(KWeatherCore::AlertInfo::Categories categories)
{
    return flagToString(categories, category_map);
}

QStringList CAPUtil::categoriesDisplayStrings(uint categories)
{
    return categoriesDisplayStrings(KWeatherCore::AlertInfo::Categories(categories));
}

QStringList CAPUtil::responseTypesStrings(KWeatherCore::AlertInfo::ResponseTypes responseTypes)
{
    return flagToString(responseTypes, response_map);
}

QStringList CAPUtil::responseTypesStrings(uint responseTypes)
{
    return responseTypesStrings(KWeatherCore::AlertInfo::ResponseTypes(responseTypes));
}

QColor CAPUtil::colorMix(const QColor& c1, const QColor& c2, double bias)
{
    return KColorUtils::mix(c1, c2, bias);
}
