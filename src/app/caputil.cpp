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
static constexpr const MapEntry<KWeatherCore::CAPAlertInfo::Severity> severity_map[] = {
    { KWeatherCore::CAPAlertInfo::Severity::Extreme, kli18n("Extreme - Extraordinary threat to life or property") },
    { KWeatherCore::CAPAlertInfo::Severity::Severe, kli18n("Severe - Significant threat to life or property") },
    { KWeatherCore::CAPAlertInfo::Severity::Moderate, kli18n("Moderate - Possible threat to life or property") },
    { KWeatherCore::CAPAlertInfo::Severity::Minor, kli18n("Minor - Minimal to no known threat to life or property") },
};

static constexpr const MapEntry<KWeatherCore::CAPAlertInfo::Urgency> urgency_map[] = {
    { KWeatherCore::CAPAlertInfo::Urgency::Immediate, kli18n("Responsive action should be taken immediately") },
    { KWeatherCore::CAPAlertInfo::Urgency::Expected, kli18n("Responsive action should be taken soon") },
    { KWeatherCore::CAPAlertInfo::Urgency::Future, kli18n("Responsive action be taken in the near future") },
    { KWeatherCore::CAPAlertInfo::Urgency::Past, kli18n("Responsive action is no longer required") },
};

static constexpr const MapEntry<KWeatherCore::CAPAlertInfo::Certainty> certainty_map[] = {
    { KWeatherCore::CAPAlertInfo::Certainty::Observed, kli18n("Occurred or ongoing") },
    { KWeatherCore::CAPAlertInfo::Certainty::Likely, kli18n("Likely (>50%)") },
    { KWeatherCore::CAPAlertInfo::Certainty::Possible, kli18n("Possible but not likely (<50%)") },
    { KWeatherCore::CAPAlertInfo::Certainty::Unlikely, kli18n("Not expected to occur") },
};

static constexpr const MapEntry<KWeatherCore::CAPAlertInfo::Category> category_map[] = {
    { KWeatherCore::CAPAlertInfo::Category::Geophysical, kli18n("Geophysical") },
    { KWeatherCore::CAPAlertInfo::Category::Meteorological, kli18n("Meteorological") },
    { KWeatherCore::CAPAlertInfo::Category::Safety, kli18n("General emergency and public safety") },
    { KWeatherCore::CAPAlertInfo::Category::Security, kli18n("Law enforcement, military, homeland and local/private security") },
    { KWeatherCore::CAPAlertInfo::Category::Rescue, kli18n("Rescue and recovery") },
    { KWeatherCore::CAPAlertInfo::Category::Fire, kli18n("Fire") },
    { KWeatherCore::CAPAlertInfo::Category::Health, kli18n("Medical and public health") },
    { KWeatherCore::CAPAlertInfo::Category::Environmental, kli18n("Pollution and other environmental event") },
    { KWeatherCore::CAPAlertInfo::Category::Transport, kli18n("Transportation") },
    { KWeatherCore::CAPAlertInfo::Category::Infrastructure, kli18n("Utility, telecommunication, other non-transport infrastructure") },
    { KWeatherCore::CAPAlertInfo::Category::CBRNE, kli18n("Chemical, Biological, Radiological, Nuclear or High-Yield Explosive threat or attack") },
    { KWeatherCore::CAPAlertInfo::Category::Other, kli18n("Other event") },
};

static constexpr const MapEntry<KWeatherCore::CAPAlertInfo::ResponseType> response_map[] = {
    { KWeatherCore::CAPAlertInfo::ResponseType::Shelter, kli18n("Take shelter in place or per instructions.") },
    { KWeatherCore::CAPAlertInfo::ResponseType::Evacuate, kli18n("Relocate as instructed.") },
    { KWeatherCore::CAPAlertInfo::ResponseType::Prepare, kli18n("Make preparations as per the instructions.") },
    { KWeatherCore::CAPAlertInfo::ResponseType::Execute, kli18n("Execute activities identified in the instructions.") },
    { KWeatherCore::CAPAlertInfo::ResponseType::Avoid, kli18n("Avoid the subject event as per the instructions.") },
    { KWeatherCore::CAPAlertInfo::ResponseType::Monitor, kli18n("Attend to information sources as described in the instructions.") },
    { KWeatherCore::CAPAlertInfo::ResponseType::Assess, kli18n("Evaluate the information in this message.") },
    { KWeatherCore::CAPAlertInfo::ResponseType::AllClear, kli18n("The subject event no longer poses a threat or concern and any follow on action is described in the instructions.") },
    { KWeatherCore::CAPAlertInfo::ResponseType::None, kli18n("No action recommended.") },
};

QString CAPUtil::severityDisplayString(KWeatherCore::CAPAlertInfo::Severity severity)
{
    return valueToString(severity, severity_map);
}

QString CAPUtil::urgencyDisplayString(KWeatherCore::CAPAlertInfo::Urgency urgency)
{
    return valueToString(urgency, urgency_map);
}

QString CAPUtil::certaintyDisplayString(KWeatherCore::CAPAlertInfo::Certainty certainty)
{
    return valueToString(certainty, certainty_map);
}

QStringList CAPUtil::categoriesDisplayStrings(KWeatherCore::CAPAlertInfo::Categories categories)
{
    return flagToString(categories, category_map);
}

QStringList CAPUtil::categoriesDisplayStrings(uint categories)
{
    return categoriesDisplayStrings(KWeatherCore::CAPAlertInfo::Categories(categories));
}

QStringList CAPUtil::responseTypesStrings(KWeatherCore::CAPAlertInfo::ResponseTypes responseTypes)
{
    return flagToString(responseTypes, response_map);
}

QStringList CAPUtil::responseTypesStrings(uint responseTypes)
{
    return responseTypesStrings(KWeatherCore::CAPAlertInfo::ResponseTypes(responseTypes));
}

// sorted by priority
struct {
    KWeatherCore::CAPAlertInfo::Categories categories;
    const char *iconName;
} static constexpr const category_icon_map[] = {
    // TODO proper icons
    { KWeatherCore::CAPAlertInfo::Category::CBRNE, "office-chart-polar-stacked" },
    { KWeatherCore::CAPAlertInfo::Category::Fire, "hotspot" },
    { KWeatherCore::CAPAlertInfo::Category::Meteorological, "cloudstatus" },
    { KWeatherCore::CAPAlertInfo::Category::Health, "cross-shape" },
    { KWeatherCore::CAPAlertInfo::Category::Geophysical, "earthquake" },
    { KWeatherCore::CAPAlertInfo::Category::Safety, "security-high-symbolic" },
    { KWeatherCore::CAPAlertInfo::Category::Security, "security-high-symbolic" },
    { KWeatherCore::CAPAlertInfo::Category::Environmental, "internet-services" },
    { KWeatherCore::CAPAlertInfo::Category::Infrastructure, "network-wireless-hotspot" },
    { KWeatherCore::CAPAlertInfo::Category::Transport, "car" },
};

QString CAPUtil::categoriesIconName(KWeatherCore::CAPAlertInfo::Categories categories)
{
    for (const auto &m : category_icon_map) {
        if ((m.categories & categories) == m.categories) {
            return QLatin1String(m.iconName);
        }
    }
    return QLatin1String("dialog-warning");
}

QString KPublicAlerts::CAPUtil::categoriesIconName(uint categories)
{
    return categoriesIconName(KWeatherCore::CAPAlertInfo::Categories(categories));
}

QColor CAPUtil::colorMix(const QColor& c1, const QColor& c2, double bias)
{
    return KColorUtils::mix(c1, c2, bias);
}
