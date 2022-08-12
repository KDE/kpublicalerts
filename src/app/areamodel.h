/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_AREAMODEL_H
#define KPUBLICALERTS_AREAMODEL_H

#include <KWeatherCore/AlertInfo>

#include <QAbstractListModel>

namespace KPublicAlerts {

/** List of target areas for a given CAP alert message info element. */
class AreaModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(KWeatherCore::AlertInfo alertInfo READ alertInfo WRITE setAlertInfo NOTIFY alertInfoChanged)

public:
    explicit AreaModel(QObject *parent = nullptr);
    ~AreaModel();

    enum Role {
        PolygonsRole = Qt::UserRole,
    };

    KWeatherCore::AlertInfo alertInfo() const;
    void setAlertInfo(const KWeatherCore::AlertInfo &info);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void alertInfoChanged();

private:
    KWeatherCore::AlertInfo m_alert;
};

}

#endif // KPUBLICALERTS_AREAMODEL_H
