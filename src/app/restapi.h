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
[[nodiscard]] QNetworkRequest alert(const QString &id);
[[nodiscard]] QNetworkRequest alerts(const QRectF &bbox);

[[nodiscard]] QNetworkRequest vapidKey();
[[nodiscard]] QNetworkRequest subscribe();
[[nodiscard]] QNetworkRequest unsubscribe(const QUuid &id);

[[nodiscard]] QNetworkRequest heartbeat(const QUuid &id);
}

}

#endif // KPUBLICALERTS_RESTAPI_H
