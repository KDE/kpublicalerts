# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.db import models
#from django.contrib.gis.db import models

# Create your models here.
class Alert(models.Model):
    issuerId = models.CharField(max_length=255)
    alertId = models.CharField(max_length=255)
    # TODO bbox = models.PolygonField()
    # TODO capData: stored in FS or in database?
    issueDate = models.DateField()
    expireDate = models.DateField()
    sourceUrl = models.CharField(max_length=255)

class Subscription(models.Model):
    # TODO bbox = models.PolygonField()
    upEndpoint = models.CharField(max_length=255)
    # TODO encryption key
