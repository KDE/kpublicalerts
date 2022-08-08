# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import json
import requests
from .models import Alert, Subscription

def notifyAlert(alert, msg):
    # TODO bbox intersection
    for subscription in Subscription.objects.all():
        print(subscription.upEndpoint, msg)
        requests.post(subscription.upEndpoint, json.dumps(msg))
    pass
