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

    CAPFeedReader('af-andma', 'https://cap-sources.s3.amazonaws.com/af-andma-en/rss.xml'),
    CAPFeedReader('ar-smn', 'https://ssl.smn.gob.ar/CAP/AR.php'),
    CAPFeedReader('at-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-austria'),
    CAPFeedReader('be-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-belgium'),
    CAPFeedReader('br-inmet', 'https://apiprevmet3.inmet.gov.br/avisos/rss'),
    CAPFeedReader('bf-meteo', 'https://cap-sources.s3.amazonaws.com/bf-meteo-en/rss.xml'),
    CAPFeedReader('bw-met', 'https://cap-sources.s3.amazonaws.com/bw-met-en/rss.xml'),
    CAPFeedReader('ca-met-service', 'https://rss.naad-adna.pelmorex.com/'),
    CAPFeedReader('cf-dmn', 'https://cap-sources.s3.amazonaws.com/cf-dmn-en/rss.xml'),
    CAPFeedReader('ch-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-switzerland'),
    CAPFeedReader('ci-sodexam', 'https://cap-sources.s3.amazonaws.com/ci-sodexam-en/rss.xml'),
    CAPFeedReader('cm-meteo', 'https://cap-sources.s3.amazonaws.com/cm-meteo-en/rss.xml'),
    CAPFeedReader('cz-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-czechia'),
    DWDFeedReader('de-dwd', 'https://opendata.dwd.de/weather/alerts/cap/COMMUNEUNION_EVENT_STAT/Z_CAP_C_EDZW_LATEST_PVW_STATUS_PREMIUMEVENT_COMMUNEUNION_MUL.zip'),
    MoWaSFeedReader('de-lhp', 'https://warnung.bund.de/bbk.lhp/hochwassermeldungen.json'),
    MoWaSFeedReader('de-mowas', 'https://warnung.bund.de/bbk.mowas/gefahrendurchsagen.json'),
    CAPFeedReader('dk-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-denmark'),
    CAPFeedReader('dz-meteo', 'https://ametvigilance.meteo.dz/rss/rss_meteo_dz.xml'),
    CAPFeedReader('ec-inamhi', 'https://cap-sources.s3.amazonaws.com/ec-inamhi-es/rss.xml'),
    CAPFeedReader('ee-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-estonia'),
    CAPFeedReader('es-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-spain'),
    CAPFeedReader('fi-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-finland'),
    CAPFeedReader('fr-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-france'),
    CAPFeedReader('gm-dwr', 'https://cap-sources.s3.amazonaws.com/gm-dwr-en/rss.xml'),
    CAPFeedReader('gy-hydromet', 'https://hydromet.gov.gy/cap/en/alerts/rss.xml'),
    CAPFeedReader('id-bmkg', 'https://signature.bmkg.go.id/alert/public/en/rss.xml'),
    CAPFeedReader('in-imd', 'https://cap-sources.s3.amazonaws.com/in-imd-en/rss.xml'),
    CAPFeedReader('it-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-italy'),
    CAPFeedReader('ke-kmd', 'https://cap-sources.s3.amazonaws.com/ke-kmd-en/rss.xml'),
    CAPFeedReader('km-anacm', 'https://cap-sources.s3.amazonaws.com/km-anacm-en/rss.xml'),
    CAPFeedReader('lt-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-lithuania'),
    CAPFeedReader('lu-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-luxembourg'),
    CAPFeedReader('lv-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-latvia'),
    CAPFeedReader('mn-namem', 'https://cap-sources.s3.amazonaws.com/mn-namem-en/rss.xml'),
    CAPFeedReader('mx-smn', 'https://smn.conagua.gob.mx/tools/PHP/feedsmn/cap.php'),
    CAPFeedReader('nl-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-netherlands'),
    CAPFeedReader('no-met', 'https://alert.met.no/alerts'),
    CAPFeedReader('nz-metservice', 'https://alerts.metservice.com/cap/rss'),
    CAPFeedReader('ph-pagasa', 'https://publicalert.pagasa.dost.gov.ph/feeds/'),
    CAPFeedReader('pl-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-poland'),
    CAPFeedReader('pt-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-portugal'),
    CAPFeedReader('py-dmh', 'https://cap-sources.s3.amazonaws.com/py-dmh-en/rss.xml'),
    CAPFeedReader('se-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-sweden'),
    CAPFeedReader('sr-meteo', 'https://cap-sources.s3.amazonaws.com/sr-meteo-en/rss.xml'),
    CAPFeedReader('ug-unma', 'https://cap-sources.s3.amazonaws.com/ug-unma-en/rss.xml'),
    CAPFeedReader('us-nws', 'https://alerts.weather.gov/cap/us.php?x=0'),
    CAPFeedReader('us-ntwc', 'https://www.tsunami.gov/events/xml/PAAQAtom.xml'),
    CAPFeedReader('us-ptwc', 'https://www.tsunami.gov/events/xml/PHEBAtom.xml'),
    CAPFeedReader('za-weathersa', 'https://caps.weathersa.co.za/Home/RssFeed'),
]

for feedReader in feedReaders:
    if arguments.only == None or arguments.only == feedReader.issuerId:
        feedReader.update()
