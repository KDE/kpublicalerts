# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from django.apps import AppConfig

class AggregatorConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'aggregator'
