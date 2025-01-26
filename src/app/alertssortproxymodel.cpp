/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "alertssortproxymodel.h"
#include "alertsmanager.h"

#include <KLocalizedString>

#include <QDebug>

using namespace KPublicAlerts;

static_assert((int)AlertsSortProxyModel::SectionTitleRole >= (int)AlertsManager::UserRole);

AlertsSortProxyModel::AlertsSortProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    sort(0, Qt::AscendingOrder);

    m_groupingInvalidationTimer.setTimerType(Qt::VeryCoarseTimer);
    m_groupingInvalidationTimer.setSingleShot(true);
    connect(&m_groupingInvalidationTimer, &QTimer::timeout, this, [this]() {
        invalidate();
        scheduleRegrouping();
    });
    scheduleRegrouping();
}

AlertsSortProxyModel::~AlertsSortProxyModel() = default;

QVariant AlertsSortProxyModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index)) {
        return {};
    }

    if (role == SectionTitleRole) {
        const auto onsetTime = QSortFilterProxyModel::data(index, AlertsManager::OnsetTimeRole).toDateTime();
        if (onsetTime.isValid() && onsetTime > QDateTime::currentDateTime()) {
            return i18n("Future");
        } else {
            return i18n("Current");
        }
    }

    return QSortFilterProxyModel::data(index, role);
}

QHash<int, QByteArray> AlertsSortProxyModel::roleNames() const
{
    auto r = QSortFilterProxyModel::roleNames();
    r.insert(SectionTitleRole, "sectionTitle");
    return r;
}

void AlertsSortProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    if (this->sourceModel()) {
        disconnect(this->sourceModel(), nullptr, this, nullptr);
    }
    QSortFilterProxyModel::setSourceModel(sourceModel);
    connect(sourceModel, &QAbstractItemModel::rowsInserted, this, &AlertsSortProxyModel::scheduleRegrouping);
    connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, &AlertsSortProxyModel::scheduleRegrouping);
    connect(sourceModel, &QAbstractItemModel::dataChanged, this, &AlertsSortProxyModel::scheduleRegrouping);
    scheduleRegrouping();
}

bool AlertsSortProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    // separate future and current alerts
    const auto now = QDateTime::currentDateTime();
    const auto lhsOnsetTime = source_left.data(AlertsManager::OnsetTimeRole).toDateTime();
    const auto rhsOnsetTime = source_right.data(AlertsManager::OnsetTimeRole).toDateTime();
    if (lhsOnsetTime.isValid() && lhsOnsetTime > now && (!rhsOnsetTime.isValid() || rhsOnsetTime < now)) {
        return true;
    }
    if (rhsOnsetTime.isValid() && rhsOnsetTime > now && (!lhsOnsetTime.isValid() || lhsOnsetTime < now)) {
        return false;
    }

    // put cancellations behind still active alerts
    const auto lhsAlert = source_left.data(AlertsManager::AlertRole).value<KWeatherCore::CAPAlertMessage>();
    const auto rhsAlert = source_right.data(AlertsManager::AlertRole).value<KWeatherCore::CAPAlertMessage>();
    if (lhsAlert.messageType() != rhsAlert.messageType()) {
        if (rhsAlert.messageType() == KWeatherCore::CAPAlertMessage::MessageType::Cancel) {
            return true;
        }
        if (lhsAlert.messageType() == KWeatherCore::CAPAlertMessage::MessageType::Cancel) {
            return false;
        }
    }

    // sort by severity and sent time
    const auto lhsInfo = source_left.data(AlertsManager::AlertInfoRole).value<KWeatherCore::CAPAlertInfo>();
    const auto rhsInfo = source_right.data(AlertsManager::AlertInfoRole).value<KWeatherCore::CAPAlertInfo>();

    if (lhsInfo.severity() == rhsInfo.severity()) {
        if (lhsAlert.sentTime() == rhsAlert.sentTime()) {
            return QSortFilterProxyModel::lessThan(source_left, source_right);
        }
        return lhsAlert.sentTime() > rhsAlert.sentTime();
    }
    return lhsInfo.severity() < rhsInfo.severity();
}

void AlertsSortProxyModel::scheduleRegrouping()
{
    if (!sourceModel()) {
        return;
    }

    const auto now = QDateTime::currentDateTime();
    QDateTime dt;
    for (int i = 0; i < rowCount(); ++i) {
        const auto idx = index(i, 0);
        const auto onsetTime = idx.data(AlertsManager::OnsetTimeRole).toDateTime();
        if (onsetTime.isValid() && onsetTime > now) {
            dt = dt.isValid() ? std::min(onsetTime, dt) : onsetTime;
        }
    }

    qDebug() << "next regrouping:" << dt;;
    if (dt.isValid()) {
        m_groupingInvalidationTimer.start(std::max(std::chrono::seconds(QDateTime::currentDateTime().secsTo(dt)), std::chrono::seconds(60)));
    }
}

#include "moc_alertssortproxymodel.cpp"
