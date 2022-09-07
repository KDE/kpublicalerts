# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.urls import path

from . import views

urlpatterns = [
    path('feeder/<str:sourceId>/alert', views.post_alert),
    path('feeder/<str:sourceId>/activeAlerts', views.post_active_alerts),
]

