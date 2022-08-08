# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import feedparser

from abstractfeedreader import AbstractFeedReader

class CAPFeedReader(AbstractFeedReader):
    feedUrl = ''

    def __init__(self, issuerId, feedUrl):
        self.issuerId = issuerId
        self.feedUrl = feedUrl

    def updateFeed(self):
        feed = feedparser.parse(self.feedUrl)
        print(feed['namespaces']) # TODO properly identify the CAP namespace
        for entry in feed['entries']:
            capSource = ''
            for link in entry['links']:
                if link['type'] == 'application/cap+xml':
                    capSource = link['href']
            self.addAlert(
                alertId = entry['cap_identifier'],
                issuedTime = entry['cap_sent'],
                capSource = capSource
            )
        print("TODO")
