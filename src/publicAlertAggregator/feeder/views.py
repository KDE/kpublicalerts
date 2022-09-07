# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import datetime
from django.contrib.gis.geos import Polygon
from django.core.files.uploadedfile import SimpleUploadedFile
from django.http import HttpResponse, HttpResponseBadRequest, HttpResponseNotFound, JsonResponse
from json import loads
from aggregator.models import Alert
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

    x1 = data['minlon']
    x2 = data['maxlon']
    y1 = data['minlat']
    y2 = data['maxlat']
    alert.bbox = Polygon.from_bbox((x1, y1, x2, y2))
    print(alert.bbox, x1, y1, x2, y2)

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
            skipNotify = False
            if alert.expireDate != None:
                # don't bother with sending removal notification if the alert is about to expire anyway
                deltaToExpiry = alert.expireDate - datetime.datetime.now(datetime.timezone.utc)
                skipNotify = deltaToExpiry.days <= 0 and deltaToExpiry.seconds <= (5*60)
            if not skipNotify:
                notifyAlert(alert, { 'removed': str(alert.id) })
            alert.delete()

    return HttpResponse()
from django.shortcuts import render
