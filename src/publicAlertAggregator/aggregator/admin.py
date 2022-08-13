# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.contrib import admin
from .models import Alert, Subscription

class AlertAdmin(admin.ModelAdmin):
    list_display = ('issuerId', 'alertId', 'issueDate', 'expireDate')

admin.site.register(Alert, AlertAdmin)
admin.site.register(Subscription)
