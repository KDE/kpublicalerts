/*
    SPDX-FileCopyrightText: 2021-2022 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "polygonsimplifier.h"
#include "geomath.h"

#include <QDebug>
#include <QRectF>

using namespace KPublicAlerts;

static KWeatherCore::CAPPolygon douglasPeucker(const KWeatherCore::CAPPolygon::const_iterator &begin, const KWeatherCore::CAPPolygon::const_iterator &end, double threshold)
{
    KWeatherCore::CAPPolygon result;
    if (std::distance(begin, end) < 3) {
        std::copy(begin, end, std::back_inserter(result));
        return result;
    }

    double maxDistance = 0.0;
    auto maxDistIt = std::next(begin);
    for (auto it = maxDistIt; it != end; ++it) {
        const auto d = GeoMath::distance((*begin), (*std::prev(end)), (*it));
        if (d > maxDistance) {
            maxDistance = d;
            maxDistIt = it;
        }
    }

    if (maxDistance >= threshold) {
        auto p = douglasPeucker(begin, std::next(maxDistIt), threshold);
        result.insert(result.end(), p.begin(), p.end());
        result.pop_back();
        p = douglasPeucker(maxDistIt, end, threshold);
        result.insert(result.end(), p.begin(), p.end());
        return result;
    }

    return KWeatherCore::CAPPolygon({*begin, (*std::prev(end))});
}

static KWeatherCore::CAPCoordinate p2c(const QPointF &p)
{
    return KWeatherCore::CAPCoordinate{ (float)p.y(), (float)p.x() };
}

KWeatherCore::CAPPolygon PolygonSimplifier::douglasPeucker(const KWeatherCore::CAPPolygon &poly, double threshold)
{
    auto result = ::douglasPeucker(poly.begin(), poly.end(), threshold);
    qDebug() << "got" << poly.size() << "dropped" << poly.size() - result.size() << "remaining" << result.size();

    // if the polygon itself is smaller than the threshold, the result can deteriorate into
    // an area-less structure, take the bounding rect in that case
    if (result.size() < 4) {
        qDebug() << "  result deteriorated, taking bounding rect instead";
        const auto bbox = GeoMath::boundingBoxForPolygon(poly);
        return KWeatherCore::CAPPolygon({ p2c(bbox.topLeft()), p2c(bbox.topRight()), p2c(bbox.bottomRight()), p2c(bbox.bottomLeft()), p2c(bbox.topLeft()) });
    }

    return result;
}
