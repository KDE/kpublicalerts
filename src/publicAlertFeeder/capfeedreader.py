# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import feedparser
import requests_cache

from abstractfeedreader import AbstractFeedReader

class CAPFeedReader(AbstractFeedReader):
    feedUrl = ''
    session = None

    def __init__(self, issuerId, feedUrl):
        self.issuerId = issuerId
        self.feedUrl = feedUrl
        self.session = requests_cache.session.CachedSession(cache_name = self.issuerId)

    def updateFeed(self):
        feed = feedparser.parse(self.feedUrl)
        for entry in feed['entries']:
            # find the link to the CAP source
            capSource = ''
            for link in entry['links']:
                if link['type'] == 'application/cap+xml':
                    capSource = link['href']
            if not capSource and len(entry['links']) == 1:
                capSource = entry['links'][0]['href']

            if not capSource:
                continue
            capData = self.session.get(capSource).content.decode('utf-8')
            self.addAlert(capSource = capSource, capData = capData)
