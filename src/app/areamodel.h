/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_AREAMODEL_H
#define KPUBLICALERTS_AREAMODEL_H

#include <KWeatherCore/CAPAlertInfo>

#include <QAbstractListModel>
#include <QRectF>

namespace KPublicAlerts {

/** List of target areas for a given CAP alert message info element. */
class AreaModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(KWeatherCore::CAPAlertInfo alertInfo READ alertInfo WRITE setAlertInfo NOTIFY alertInfoChanged)
    Q_PROPERTY(QString allAreaDescriptions READ allAreaDescriptions NOTIFY alertInfoChanged)

public:
    explicit AreaModel(QObject *parent = nullptr);
    ~AreaModel();

    enum Role {
        PolygonsRole = Qt::UserRole,
        CirclesRole,
    };

    KWeatherCore::CAPAlertInfo alertInfo() const;
    void setAlertInfo(const KWeatherCore::CAPAlertInfo &info);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QRectF boundingBox() const;
    Q_INVOKABLE QPointF center() const;
    Q_INVOKABLE float zoomLevel(float width, float height) const;

    /** All area names concatenated, as those can be too many for a QML Repeater */
    QString allAreaDescriptions() const;
Q_SIGNALS:
    void alertInfoChanged();

private:
    KWeatherCore::CAPAlertInfo m_alert;
};

}

#endif // KPUBLICALERTS_AREAMODEL_H
