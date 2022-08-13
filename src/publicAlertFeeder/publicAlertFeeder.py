# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from capfeedreader import CAPFeedReader
from mowasfeedreader import MoWaSFeedReader
import requests_cache

requests_cache.install_cache('public-alert-feeder-cache')

feedReaders = [
    CAPFeedReader('at-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-austria'),
    MoWaSFeedReader(),
    CAPFeedReader('fr-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-france'),
    CAPFeedReader('us-nws', 'https://alerts.weather.gov/cap/us.php?x=0'),
]

for feedReader in feedReaders:
    feedReader.update()
