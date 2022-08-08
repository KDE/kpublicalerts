# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.db import models
#from django.contrib.gis.db import models

# Alert records
def alert_upload_path(instance, filename):
    return f"alerts/{instance.issuerId}/{filename}"

class Alert(models.Model):
    issuerId = models.CharField(max_length=255)
    alertId = models.CharField(max_length=255)
    # TODO bbox = models.PolygonField()
    capData = models.FileField(upload_to=alert_upload_path, null=True)
    issueDate = models.DateTimeField()
    expireDate = models.DateTimeField(null=True)
    sourceUrl = models.CharField(max_length=255, null=True)

    class Meta:
        constraints = [models.UniqueConstraint('issuerId', 'alertId', name='issuerId_alertId_unique')]

# Subscription records
class Subscription(models.Model):
    # TODO bbox = models.PolygonField()
    upEndpoint = models.CharField(max_length=255)
    # TODO encryption key
