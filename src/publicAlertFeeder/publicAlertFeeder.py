# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import argparse
from capfeedreader import CAPFeedReader
from dwdfeedreader import DWDFeedReader
from mowasfeedreader import MoWaSFeedReader

parser = argparse.ArgumentParser(description='CAP feed loader.')
parser.add_argument('--only', type=str, help='only read a single feed')
arguments = parser.parse_args()

feedReaders = [
    # interesting data, but past-only :(
    #CAPFeedReader('un-gdacs', 'https://gdacs.org/xml/gdacs_cap.xml'),

    CAPFeedReader('ar-smn', 'https://ssl.smn.gob.ar/CAP/AR.php'),
    CAPFeedReader('at-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-austria'),
    CAPFeedReader('be-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-belgium'),
    CAPFeedReader('br-inmet', 'https://apiprevmet3.inmet.gov.br/avisos/rss'),
    DWDFeedReader('de-dwd', 'https://opendata.dwd.de/weather/alerts/cap/COMMUNEUNION_EVENT_STAT/Z_CAP_C_EDZW_LATEST_PVW_STATUS_PREMIUMEVENT_COMMUNEUNION_MUL.zip'),
    MoWaSFeedReader('de-mowas', 'https://warnung.bund.de/bbk.mowas/gefahrendurchsagen.json'),
    CAPFeedReader('es-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-spain'),
    CAPFeedReader('fr-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-france'),
    CAPFeedReader('gm-dwr', 'https://cap-sources.s3.amazonaws.com/gm-dwr-en/rss.xml'),
    CAPFeedReader('in-imd', 'https://cap-sources.s3.amazonaws.com/in-imd-en/rss.xml'),
    CAPFeedReader('no-met', 'https://alert.met.no/alerts'),
    CAPFeedReader('us-nws', 'https://alerts.weather.gov/cap/us.php?x=0'),
    CAPFeedReader('us-ntwc', 'https://www.tsunami.gov/events/xml/PAAQAtom.xml'),
    CAPFeedReader('us-ptwc', 'https://www.tsunami.gov/events/xml/PHEBAtom.xml'),
]

for feedReader in feedReaders:
    if arguments.only == None or arguments.only == feedReader.issuerId:
        feedReader.update()
