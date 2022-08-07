# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.http import HttpResponse, HttpResponseBadRequest, HttpResponseNotFound, JsonResponse
from json import loads
from .models import Subscription

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
