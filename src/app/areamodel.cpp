/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "areamodel.h"

#include <KWeatherCore/CAPArea>

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

    switch (role) {
        case Qt::DisplayRole:
            return m_alert.areas()[index.row()].description();
        case PolygonsRole:
        {
            const auto &polygons = m_alert.areas()[index.row()].polygons();
            if (polygons.empty()) {
                return {};
            }

            QList<QList<QGeoCoordinate>> polys;
            polys.reserve(polygons.size());
            for (const auto &p : polygons) {
                QList<QGeoCoordinate> coords;
                coords.reserve(p.size());
                std::transform(p.begin(), p.end(), std::back_inserter(coords), [](const auto &c) { return QGeoCoordinate(c.second, c.first); });
                polys.push_back(coords);
            }
            return QVariant::fromValue(polys);
        }
    }

    return {};
}

QHash<int, QByteArray> AreaModel::roleNames() const
{
    auto n = QAbstractListModel::roleNames();
    n.insert(PolygonsRole, "polygons");
    return n;
}
