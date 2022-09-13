/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "geomath.h"

#include <QRectF>

using namespace KPublicAlerts;

// see https://en.wikipedia.org/wiki/Haversine_formula
double GeoMath::distance(double lat1, double lon1, double lat2, double lon2)
{
    constexpr const auto earthRadius = 6371000.0; // in meters

    const auto d_lat = degToRad(lat1 - lat2);
    const auto d_lon = degToRad(lon1 - lon2);

    const auto a = std::pow(std::sin(d_lat / 2.0), 2) + std::cos(degToRad(lat1)) * std::cos(degToRad(lat2)) * std::pow(std::sin(d_lon / 2.0f), 2);
    return 2.0 * earthRadius * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
}

QRectF GeoMath::boundingBoxForCircle(double lat, double lon, double radius)
{
    const auto dlon = radius / GeoMath::distance(lat, 0.0, lat, 1.0);
    const auto dlat = radius / GeoMath::distance(0.0, lon, 1.0, lon);
    return QRectF(QPointF(lon - dlon, lat - dlat), QPointF(lon + dlon, lat + dlat));
}

QPointF GeoMath::mercatorProject(double lat, double lon, double zoom)
{
    const auto x = (256.0 / (2.0 * M_PI)) * std::pow(2.0, zoom) * (degToRad(lon) + M_PI);
    const auto y = (256.0 / (2.0 * M_PI)) * std::pow(2.0, zoom) * (M_PI - std::log(std::tan(M_PI / 4.0 + degToRad(lat) / 2.0)));
    return QPointF(x, y);
}
