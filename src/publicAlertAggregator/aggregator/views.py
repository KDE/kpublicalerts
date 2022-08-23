# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.contrib.gis.geos import Polygon
from django.http import HttpResponse, HttpResponseBadRequest, HttpResponseNotFound, HttpResponsePermanentRedirect, HttpResponseRedirect, JsonResponse
from json import loads
from .models import Alert, Subscription

#
# Public subscription API
#

# add new subscription
def post_subscription(request):
    if request.method != 'POST':
        return HttpResponseBadRequest('wrong HTTP method')
    # TODO validate input
    print(request, request.body)
    data = loads(request.body)
    y1 = float(data['minlat'])
    y2 = float(data['maxlat'])
    x1 = float(data['minlon'])
    x2 = float(data['maxlon'])
    bbox = Polygon.from_bbox((x1, y1, x2, y2))
    s = Subscription(upEndpoint = data['endpoint'], bbox = bbox)
    s.save()
    print(s.id);
    return JsonResponse({ 'id': s.id })

# unsubscribe
def delete_subscription(request, identifier):
    if request.method != 'DELETE':
        return HttpResponseBadRequest('wrong HTTP method')
    try:
        s = Subscription.objects.get(id = identifier)
    except:
        return HttpResponseNotFound('no such subscription')
    s.delete()
    return HttpResponse()


#
# Public alert query API
#

# get alert CAP data
def get_alert_cap_data(request, identifier):
    if request.method != 'GET':
        return HttpResponseBadRequest('wrong HTTP method')
    try:
        a = Alert.objects.get(id = identifier)
    except:
        return HttpResponseNotFound('no such alert')

    if a.sourceUrl:
        return HttpResponseRedirect(a.sourceUrl)
    elif a.capData:
        return HttpResponsePermanentRedirect(a.capData.url)

    return HttpResponseBadRequest('alert without CAP data?')

# list all alerts in a given area
def get_alerts(request):
    if request.method != 'GET':
        return HttpResponseBadRequest('wrong HTTP method')

    try:
        y1 = float(request.GET.get('minlat'))
        y2 = float(request.GET.get('maxlat'))
        x1 = float(request.GET.get('minlon'))
        x2 = float(request.GET.get('maxlon'))
        request_bbox = Polygon.from_bbox((x1, y1, x2, y2))
    except (ValueError, TypeError):
        return HttpResponseBadRequest('invalid bounding box')

    res = []
    for alert in Alert.objects.filter(bbox__intersects = request_bbox):
        res.append(str(alert.id))
    return JsonResponse(res, safe=False)
