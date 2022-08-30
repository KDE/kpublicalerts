/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_ALERTSSORTPROXYMODEL_H
#define KPUBLICALERTS_ALERTSSORTPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace KPublicAlerts {

/** Sort alerts by severity and urgency/relevance. */
class AlertsSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit AlertsSortProxyModel(QObject *parent = nullptr);
    ~AlertsSortProxyModel();

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};

}

#endif // KPUBLICALERTS_ALERTSSORTPROXYMODEL_H
