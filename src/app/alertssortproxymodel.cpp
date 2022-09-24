/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "alertssortproxymodel.h"
#include "alertsmanager.h"

#include <QDebug>

using namespace KPublicAlerts;

AlertsSortProxyModel::AlertsSortProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    sort(0, Qt::AscendingOrder);
}

AlertsSortProxyModel::~AlertsSortProxyModel() = default;

bool AlertsSortProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const auto lhsInfo = source_left.data(AlertsManager::AlertInfoRole).value<KWeatherCore::CAPAlertInfo>();
    const auto rhsInfo = source_right.data(AlertsManager::AlertInfoRole).value<KWeatherCore::CAPAlertInfo>();

    if (lhsInfo.severity() == rhsInfo.severity()) {
        const auto lhsAlert = source_left.data(AlertsManager::AlertRole).value<KWeatherCore::CAPAlertMessage>();
        const auto rhsAlert = source_right.data(AlertsManager::AlertRole).value<KWeatherCore::CAPAlertMessage>();
        if (lhsAlert.sentTime() == rhsAlert.sentTime()) {
            return QSortFilterProxyModel::lessThan(source_left, source_right);
        }
        return lhsAlert.sentTime() > rhsAlert.sentTime();
    }
    return lhsInfo.severity() < rhsInfo.severity();
}
