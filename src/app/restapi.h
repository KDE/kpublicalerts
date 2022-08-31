/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPUBLICALERTS_RESTAPI_H
#define KPUBLICALERTS_RESTAPI_H

class QNetworkRequest;
class QRectF;
class QString;
class QUuid;

namespace KPublicAlerts {

/** REST API of the server part of this. */
namespace RestApi
{
QNetworkRequest alert(const QString &id);
QNetworkRequest alerts(const QRectF &bbox);

QNetworkRequest subscribe();
QNetworkRequest unsubscribe(const QUuid &id);
}

}

#endif // KPUBLICALERTS_RESTAPI_H
