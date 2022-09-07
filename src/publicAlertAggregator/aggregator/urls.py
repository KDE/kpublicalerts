# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.urls import path

from . import views

urlpatterns = [
    path('alert/<uuid:identifier>', views.get_alert_cap_data),
    path('alerts', views.get_alerts),

    path('subscription', views.post_subscription),
    path('subscription/<uuid:identifier>', views.delete_subscription),
]
