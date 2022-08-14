# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.db import models
from django.dispatch import receiver
from django.contrib.gis.db import models
import os
import uuid

# Alert records
def alert_upload_path(instance, filename):
    return f"alerts/{instance.issuerId}/{filename}"

class Alert(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    issuerId = models.CharField(max_length=255)
    alertId = models.CharField(max_length=255)
    bbox = models.PolygonField()
    capData = models.FileField(upload_to=alert_upload_path, null=True)
    issueDate = models.DateTimeField()
    expireDate = models.DateTimeField(null=True)
    sourceUrl = models.CharField(max_length=255, null=True)

    class Meta:
        constraints = [models.UniqueConstraint('issuerId', 'alertId', name='issuerId_alertId_unique')]

@receiver(models.signals.post_delete, sender=Alert)
def auto_delete_capdata_on_delete(sender, instance, **kwargs):
    if instance.capData:
        if os.path.isfile(instance.capData.path):
            os.remove(instance.capData.path)

# Subscription records
class Subscription(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    bbox = models.PolygonField()
    upEndpoint = models.CharField(max_length=255)
    # TODO encryption key
