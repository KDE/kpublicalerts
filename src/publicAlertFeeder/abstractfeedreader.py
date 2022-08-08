# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import json
import requests

class AbstractFeedReader:
    aggregatorBaseUrl = 'http://localhost:8000/aggregator/'
    issuerId = ''

    def addAlert(self, alertId, issuedTime, capSource = None, capData = None, expireTime = None):
        alert = {}
        alert['issuerId'] = self.issuerId
        alert['alertId'] = alertId
        alert['issueDate'] = issuedTime
        if expireTime:
            alert['expireTime'] = expireTime
        if capData:
            alert['capData'] = capData
        if capSource:
            alert['capSource'] = capSource

        requests.post(f"{self.aggregatorBaseUrl}feeder/{self.issuerId}/alert", json=alert)
