# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.contrib.gis import admin
from .models import Alert, Subscription

class AlertAdmin(admin.GISModelAdmin):
    list_display = ('sourceId', 'alertId', 'issueTime', 'expireTime')

admin.site.register(Alert, AlertAdmin)
admin.site.register(Subscription, admin.GISModelAdmin)
