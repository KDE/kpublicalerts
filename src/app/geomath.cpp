/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "geomath.h"

#include <QLineF>
#include <QPolygonF>
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

double GeoMath::distance(KWeatherCore::CAPCoordinate l1, KWeatherCore::CAPCoordinate l2, KWeatherCore::CAPCoordinate p)
{
    QLineF line(QPointF(l1.longitude, l1.latitude), QPointF(l2.longitude, l2.latitude));
    const auto len = line.length();
    if (len == 0.0) {
        return distance(l1.latitude, l1.longitude, p.latitude, p.longitude);
    }

    // project p on a line extending the line segment given by @p l1 and @p l2, and clamp to that to the segment
    QPointF pf(p.longitude, p.latitude);
    const auto r = qBound(0.0, QPointF::dotProduct(pf - line.p1(), line.p2() - line.p1()) / (len*len), 1.0);
    const auto intersection = line.p1() + r * (line.p2() - line.p1());
    return distance(intersection.y(), intersection.x(), p.latitude, p.longitude);
}

QRectF GeoMath::boundingBoxForPolygon(const KWeatherCore::CAPPolygon& poly)
{
    float minlon = 180.0f;
    float maxlon = -180.0f;
    float minlat = 90.0f;
    float maxlat = -90.0f;

    for (const auto &p : poly) {
        minlon = std::min(p.longitude, minlon);
        maxlon = std::max(p.longitude, maxlon);
        minlat = std::min(p.latitude, minlat);
        maxlat = std::max(p.latitude, maxlat);
    }

    return QRectF(QPointF(minlon, minlat), QPointF(maxlon, maxlat));
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

bool GeoMath::intersects(const KWeatherCore::CAPPolygon &poly, const QRectF &box)
{
    QPolygonF p;
    p.reserve(poly.size());
    for (const auto &c : poly) {
        p.push_back({c.longitude, c.latitude});
    }
    return p.intersects(box);
}

bool GeoMath::intersects(const KWeatherCore::CAPCircle &circle, const QRectF &box)
{
    // TODO - do this more precisely
    return boundingBoxForCircle(circle.latitude, circle.longitude, circle.radius).intersects(box);
}
