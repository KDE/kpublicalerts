# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.urls import path

from . import views

urlpatterns = [
    path('subscription', views.post_subscription),
    path('subscription/<int:identifier>', views.delete_subscription)
]
