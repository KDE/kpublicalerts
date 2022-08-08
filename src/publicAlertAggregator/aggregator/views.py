# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

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
    s = Subscription(upEndpoint = data['endpoint'])
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

    # TODO consider bbox
    res = []
    for alert in Alert.objects.all():
        res.append(str(alert.id))
    return JsonResponse(res, safe=False)
