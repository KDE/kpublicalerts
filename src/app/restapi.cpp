/*
 * SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "restapi.h"

#include <QCoreApplication>
#include <QNetworkRequest>
#include <QRectF>
#include <QUrlQuery>
#include <QUuid>

using namespace Qt::Literals;
using namespace KPublicAlerts;

static QUrl baseUrl()
{
    QUrl url;
    url.setScheme(u"https"_s);
    url.setHost(u"alerts.kde.org"_s);
    url.setPort(443);
    return url;
}

constexpr inline auto BASE_PATH = "/"_L1;

[[nodiscard]] static QNetworkRequest makeRequest(const QUrl &url)
{
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, (QCoreApplication::applicationName() + '/'_L1 + QCoreApplication::applicationVersion()).toUtf8());
    return req;
}

QNetworkRequest RestApi::alert(const QString &id)
{
    auto url = baseUrl();
    url.setPath(BASE_PATH + "alert/"_L1 + id);
    return makeRequest(url);
}

QNetworkRequest RestApi::alerts(const QRectF &bbox)
{
    auto url = baseUrl();
    url.setPath(BASE_PATH + "alert/area"_L1);

    QUrlQuery query;
    query.addQueryItem(u"min_lat"_s, QString::number(bbox.top()));
    query.addQueryItem(u"max_lat"_s, QString::number(bbox.bottom()));
    query.addQueryItem(u"min_lon"_s, QString::number(bbox.left()));
    query.addQueryItem(u"max_lon"_s, QString::number(bbox.right()));
    url.setQuery(query);

    return makeRequest(url);
}

QNetworkRequest RestApi::vapidKey()
{
    auto url = baseUrl();
    url.setPath(BASE_PATH + "subscription/"_L1);
    QUrlQuery query;
    query.addQueryItem(u"type"_s, u"webpush"_s);
    url.setQuery(query);
    return makeRequest(url);
}

QNetworkRequest RestApi::subscribe()
{
    auto url = baseUrl();
    url.setPath(BASE_PATH + "subscription/"_L1);
    auto req = makeRequest(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, u"application/json"_s);
    return req;
}

QNetworkRequest RestApi::unsubscribe(const QUuid &id)
{
    auto url = baseUrl();
    url.setPath(BASE_PATH + "subscription/"_L1);
    QUrlQuery query;
    query.addQueryItem(u"subscription_id"_s, id.toString(QUuid::WithoutBraces));
    url.setQuery(query);
    auto req = makeRequest(url);
    return req;
}

QNetworkRequest RestApi::heartbeat(const QUuid &id)
{
    auto url = baseUrl();
    url.setPath(BASE_PATH + "subscription/"_L1);
    QUrlQuery query;
    query.addQueryItem(u"subscription_id"_s, id.toString(QUuid::WithoutBraces));
    url.setQuery(query);
    auto req = makeRequest(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, u"application/json"_s);
    return req;
}
