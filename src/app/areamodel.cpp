/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "areamodel.h"
#include "geomath.h"
#include "polygonsimplifier.h"

#include <KWeatherCore/CAPArea>

#include <QDebug>
#include <QGeoCoordinate>

#include <span>

using namespace Qt::Literals;
using namespace KPublicAlerts;

AreaModel::AreaModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

AreaModel::~AreaModel() = default;

KWeatherCore::CAPAlertInfo AreaModel::alertInfo() const
{
    return m_alert;
}

void AreaModel::setAlertInfo(const KWeatherCore::CAPAlertInfo &info)
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
                KWeatherCore::CAPPolygon simplifiedPoly;
                std::span<const KWeatherCore::CAPCoordinate> polyView;
                if (p.size() > 500) {
                    const auto bbox = GeoMath::boundingBoxForPolygon(p);
                    const auto d = GeoMath::distance(bbox.topLeft().y(), bbox.topLeft().x(), bbox.bottomRight().y(), bbox.bottomRight().x());
                    simplifiedPoly = PolygonSimplifier::douglasPeucker(p, std::max(500.0, d * 0.005));
                    polyView = std::span<const KWeatherCore::CAPCoordinate>(simplifiedPoly.data(), simplifiedPoly.size());
                } else {
                    polyView = std::span<const KWeatherCore::CAPCoordinate>(p.data(), p.size());
                }

                QList<QGeoCoordinate> coords;
                coords.reserve(polyView.size());
                std::transform(polyView.begin(), polyView.end(), std::back_inserter(coords), [](const auto &c) {
                    return QGeoCoordinate(c.latitude, c.longitude);

                });
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
    QRectF bbox;
    for (const auto &area : m_alert.areas()) {
        for (const auto &poly : area.polygons()) {
            bbox |= GeoMath::boundingBoxForPolygon(poly);
        }

        for (const auto &circle : area.circles()) {
            // CAP radius is given in kilometers, GeoMath expects meters
            bbox |= GeoMath::boundingBoxForCircle(circle.latitude, circle.longitude, circle.radius * 1000.0);
        }
    }
    return bbox;
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

QString AreaModel::allAreaDescriptions() const
{
    QStringList l;
    l.reserve(m_alert.areas().size());
    std::ranges::transform(m_alert.areas(), std::back_inserter(l), [](const auto &area) {
        if (area.description().startsWith("polygon"_L1, Qt::CaseInsensitive) || area.description().endsWith("polygon"_L1, Qt::CaseInsensitive)) {
            return QString();
        }
        return area.description();
    });
    l.erase(std::remove_if(l.begin(), l.end(), std::mem_fn(&QString::isEmpty)), l.end());
    return l.join(QLatin1Char('\n'));
}

#include "moc_areamodel.cpp"
