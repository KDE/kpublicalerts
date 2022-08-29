# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import datetime
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

            # if we have expiry data available here already, check that
            # to avoid additional downloads
            try:
                expireTime = datetime.datetime.fromisoformat(entry.get('cap_expires'))
                if expireTime != None and expireTime < datetime.datetime.now(datetime.timezone.utc):
                    print(f"{self.issuerId} - not downloading alert {capSource} expired on {expireTime}")
                    continue
            except ValueError as e:
                print(f"Failed to parse expiry time: {entry.get('cap_expires')}")
            except TypeError as e:
                pass

            req = self.session.get(capSource)
            if not req.ok:
                print(f"Fetch error {req.status_code}: {capSource}")
                continue

            capData = req.content.decode('utf-8')
            self.addAlert(capSource = capSource, capData = capData)
