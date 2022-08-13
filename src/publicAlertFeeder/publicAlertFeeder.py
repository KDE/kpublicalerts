# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import argparse
from capfeedreader import CAPFeedReader
from mowasfeedreader import MoWaSFeedReader
import requests_cache

parser = argparse.ArgumentParser(description='CAP feed loader.')
parser.add_argument('--only', type=str, help='only read a single feed')
arguments = parser.parse_args()

requests_cache.install_cache('public-alert-feeder-cache')

feedReaders = [
    CAPFeedReader('at-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-austria'),
    MoWaSFeedReader(),
    CAPFeedReader('fr-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-france'),
    CAPFeedReader('in-imd', 'https://cap-sources.s3.amazonaws.com/in-imd-en/rss.xml'),
    CAPFeedReader('us-nws', 'https://alerts.weather.gov/cap/us.php?x=0'),
    CAPFeedReader('us-ntwc', 'https://www.tsunami.gov/events/xml/PAAQAtom.xml'),
    CAPFeedReader('us-ptwc', 'https://www.tsunami.gov/events/xml/PHEBAtom.xml'),
]

for feedReader in feedReaders:
    if arguments.only == None or arguments.only == feedReader.issuerId:
        feedReader.update()
