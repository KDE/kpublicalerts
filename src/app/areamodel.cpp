/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "areamodel.h"
#include "geomath.h"

#include <KWeatherCore/CAPArea>

#include <QDebug>
#include <QGeoCoordinate>

using namespace KPublicAlerts;

AreaModel::AreaModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

AreaModel::~AreaModel() = default;

KWeatherCore::AlertInfo AreaModel::alertInfo() const
{
    return m_alert;
}

void AreaModel::setAlertInfo(const KWeatherCore::AlertInfo &info)
{
    beginResetModel();
    m_alert = info;
    endResetModel();
}

int AreaModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_alert.areas().size();
}

QVariant AreaModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index)) {
        return {};
    }

    const auto &area = m_alert.areas()[index.row()];
    switch (role) {
        case Qt::DisplayRole:
            return area.description();
        case PolygonsRole:
        {
            const auto &polygons = area.polygons();
            if (polygons.empty()) {
                return {};
            }

            QList<QList<QGeoCoordinate>> polys;
            polys.reserve(polygons.size());
            for (const auto &p : polygons) {
                QList<QGeoCoordinate> coords;
                coords.reserve(p.size());
                std::transform(p.begin(), p.end(), std::back_inserter(coords), [](const auto &c) { return QGeoCoordinate(c.first, c.second); });
                polys.push_back(coords);
            }
            return QVariant::fromValue(polys);
        }
        case CirclesRole:
        {
            QList<KWeatherCore::CAPCircle> res;
            const auto &circles = area.circles();
            res.reserve(circles.size());
            std::copy(circles.begin(), circles.end(), std::back_inserter(res));
            return QVariant::fromValue(res);
        }
    }

    return {};
}

QHash<int, QByteArray> AreaModel::roleNames() const
{
    auto n = QAbstractListModel::roleNames();
    n.insert(PolygonsRole, "polygons");
    n.insert(CirclesRole, "circles");
    return n;
}

QRectF AreaModel::boundingBox() const
{
    float minlon = 180.0f;
    float maxlon = -180.0f;
    float minlat = 90.0f;
    float maxlat = -90.0f;

    for (const auto &area : m_alert.areas()) {
        for (const auto &poly : area.polygons()) {
            for (const auto &p : poly) {
                minlon = std::min(p.second, minlon);
                maxlon = std::max(p.second, maxlon);
                minlat = std::min(p.first, minlat);
                maxlat = std::max(p.first, maxlat);
            }
        }

        for (const auto &circle : area.circles()) {
            const auto bbox = GeoMath::boundingBoxForCircle(circle.latitude, circle.longitude, circle.radius);
            minlon = std::min<float>(minlon, bbox.left());
            maxlon = std::max<float>(maxlon, bbox.right());
            minlat = std::min<float>(minlat, bbox.bottom());
            maxlat = std::min<float>(maxlat, bbox.top());
        }
    }

    return QRectF(QPointF(minlon, minlat), QPointF(maxlon, maxlat));
}

QPointF AreaModel::center() const
{
    return boundingBox().center();
}

float AreaModel::zoomLevel(float width, float height) const
{
    const auto bbox = boundingBox();
    const auto p1 = GeoMath::mercatorProject(bbox.bottomLeft().y(), bbox.bottomLeft().x(), 1.0);
    const auto p2 = GeoMath::mercatorProject(bbox.topRight().y(), bbox.topRight().x(), 1.0);

    const auto zx = std::log2((width / (p2.x() - p1.x())));
    const auto zy = std::log2((height / (p2.y() - p1.y())));
    const auto z = std::min(zx, zy);

    return z >= 1.0 && z <= 22.0 ? z : 5.0;
}
