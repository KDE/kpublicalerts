# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import json
import requests
from .models import Alert, Subscription

def notifyAlert(alert, msg):
    for subscription in Subscription.objects.filter(bbox__intersects = alert.bbox):
        print(subscription.upEndpoint, msg)
        requests.post(subscription.upEndpoint, json.dumps(msg))
    pass
