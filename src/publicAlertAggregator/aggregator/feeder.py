# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.core.files.uploadedfile import SimpleUploadedFile
from django.http import HttpResponse, HttpResponseBadRequest, HttpResponseNotFound, JsonResponse
from json import loads
from .models import Alert
from .notify import notifyAlert

#
# API to be used by the feeder process
# ### must be behind authentication
#

# add new alert
def post_alert(request, sourceId):
    # we could also implement DELETE here, should a feeder need that
    if request.method != 'POST':
        return HttpResponseBadRequest('wrong HTTP method')
    data = loads(request.body)

    # TODO input validation

    # check whether this already exists
    alerts = Alert.objects.filter(issuerId = sourceId, alertId = data['alertId'])
    if len(alerts) == 1:
        alert = alerts[0]
        # TODO look for changes and update/notifiy as needed
        return HttpResponse()

    # new alert
    alert = Alert(issuerId = sourceId, alertId = data['alertId'], issueDate = data['issueTime'], expireDate = data.get('expireTime'), sourceUrl = data.get('capSource'))
    if 'capData' in data:
        alert.capData = SimpleUploadedFile(f"{data['alertId']}.xml", data['capData'].encode('utf-8'), 'application/xml')
    alert.save()
    notifyAlert(alert, { 'added': str(alert.id) })
    return JsonResponse({ 'id': alert.id })

# set all active alerts for a source
def post_active_alerts(request, sourceId):
    if request.method != 'POST':
        return HttpResponseBadRequest('wrong HTTP method')
    alertIds = loads(request.body)
    print(sourceId, alertIds)
    alerts = Alert.objects.filter(issuerId = sourceId)
    for alert in alerts:
        if alert.alertId not in alertIds:
            # TODO notify about removal, if it wouldn't expire anyway shortly
            alert.delete()

    return HttpResponse()
