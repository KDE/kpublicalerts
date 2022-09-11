/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "restapi.h"

#include <QNetworkRequest>
#include <QRectF>
#include <QUrlQuery>
#include <QUuid>

using namespace KPublicAlerts;

static QUrl baseUrl()
{
    QUrl url;
    // TODO
    url.setScheme(QStringLiteral("https"));
    url.setHost(QStringLiteral("volkerkrause.eu"));
    url.setPort(443);
    return url;
}

static QLatin1String basePath()
{
    return QLatin1String("/public-alerts/aggregator/");
}

QNetworkRequest RestApi::alert(const QString &id)
{
    auto url = baseUrl();
    url.setPath(basePath() + QLatin1String("alert/") + id);
    return QNetworkRequest(url);
}

QNetworkRequest RestApi::alerts(const QRectF &bbox)
{
    auto url = baseUrl();
    url.setPath(basePath() + QLatin1String("alerts"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("minlat"), QString::number(bbox.top()));
    query.addQueryItem(QStringLiteral("maxlat"), QString::number(bbox.bottom()));
    query.addQueryItem(QStringLiteral("minlon"), QString::number(bbox.left()));
    query.addQueryItem(QStringLiteral("maxlon"), QString::number(bbox.right()));
    url.setQuery(query);

    return QNetworkRequest(url);
}

QNetworkRequest RestApi::subscribe()
{
    auto url = baseUrl();
    url.setPath(basePath() + QLatin1String("subscription"));
    return QNetworkRequest(url);
}

QNetworkRequest RestApi::unsubscribe(const QUuid &id)
{
    auto url = baseUrl();
    url.setPath(basePath() + QLatin1String("subscription/") + id.toString(QUuid::WithoutBraces));
    return QNetworkRequest(url);
}
