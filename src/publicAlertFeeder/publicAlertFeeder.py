# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from capfeedreader import CAPFeedReader
from mowasfeedreader import MoWaSFeedReader

feedReaders = [
    CAPFeedReader('at-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-austria'),
    MoWaSFeedReader(),
    CAPFeedReader('fr-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-france'),
]

for feedReader in feedReaders:
    feedReader.updateFeed()
