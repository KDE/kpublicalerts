/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_GEOMATH_H
#define KPUBLICALERTS_GEOMATH_H

#include <KWeatherCore/CAPArea>

#include <cmath>

class QPointF;
class QRectF;

namespace KPublicAlerts {

/** Geographic math computations. */
namespace GeoMath
{

constexpr inline double degToRad(double deg)
{
    return deg / 180.0 * M_PI;
}

/** Distance in meters between two points. */
double distance(double lat1, double lon1, double lat2, double lon2);

/** Distance in meters between a line segment and a point. */
double distance(KWeatherCore::CAPCoordinate l1, KWeatherCore::CAPCoordinate l2, KWeatherCore::CAPCoordinate p);

QRectF boundingBoxForPolygon(const KWeatherCore::CAPPolygon &poly);
QRectF boundingBoxForCircle(double lat, double lon, double radius);

QPointF mercatorProject(double lat, double lon, double zoom);
}

}

#endif // KPUBLICALERTS_GEOMATH_H
