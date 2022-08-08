# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.urls import path

from . import feeder
from . import views

urlpatterns = [
    path('alert/<int:identifier>', views.get_alert_cap_data),
    path('alerts', views.get_alerts),

    path('subscription', views.post_subscription),
    path('subscription/<int:identifier>', views.delete_subscription),

    path('feeder/<str:sourceId>/alert', feeder.post_alert),
    path('feeder/<str:sourceId>/activeAlerts', feeder.post_active_alerts),
]
