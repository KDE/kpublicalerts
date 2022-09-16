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
    CAPFeedReader('al-igewe', 'https://cap-sources.s3.amazonaws.com/al-igewe-en/rss.xml'),
    CAPFeedReader('ar-smn', 'https://ssl.smn.gob.ar/CAP/AR.php'),
    CAPFeedReader('at-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-austria'),
    CAPFeedReader('ba-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-bosnia-herzegovina'),
    #CAPFeedReader('bb-dem', 'https://brb-secondary.capews.com/capews/public/cap/ZGV7BH8QABtsUXcYRgx6QUd2enhTSA=='), # TODO no <area> data?
    CAPFeedReader('be-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-belgium'),
    CAPFeedReader('bg-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-bulgaria'),
    CAPFeedReader('br-inmet', 'https://apiprevmet3.inmet.gov.br/avisos/rss'),
    CAPFeedReader('bf-meteo', 'https://cap-sources.s3.amazonaws.com/bf-meteo-en/rss.xml'),
    CAPFeedReader('bw-met', 'https://cap-sources.s3.amazonaws.com/bw-met-en/rss.xml'),
    CAPFeedReader('ca-met-service', 'https://rss.naad-adna.pelmorex.com/'),
    CAPFeedReader('cd-mettelsat', 'https://cap-sources.s3.amazonaws.com/cd-mettelsat-en/rss.xml'),
    CAPFeedReader('cf-dmn', 'https://cap-sources.s3.amazonaws.com/cf-dmn-en/rss.xml'),
    CAPFeedReader('cg-anac', 'https://cap-sources.s3.amazonaws.com/cg-anac-en/rss.xml'),
    CAPFeedReader('ch-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-switzerland'),
    CAPFeedReader('ci-sodexam', 'https://cap-sources.s3.amazonaws.com/ci-sodexam-en/rss.xml'),
    CAPFeedReader('cm-meteo', 'https://cap-sources.s3.amazonaws.com/cm-meteo-en/rss.xml'),
    #CAPFeedReader('cn-cma', 'http://alert-feed.worldweather.org/cn-cma-xx/rss.xml'), # TODO needs boundary polygons for "CPEAS Geographic Code"
    CAPFeedReader('cr-imn', 'https://cap-sources.s3.amazonaws.com/cr-imn-en/rss.xml'),
    CAPFeedReader('cw-meteo', 'https://cap-sources.s3.amazonaws.com/cw-meteo-en/rss.xml'),
    CAPFeedReader('cy-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-cyprus'),
    CAPFeedReader('cz-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-czechia'),
    DWDFeedReader('de-dwd', 'https://opendata.dwd.de/weather/alerts/cap/COMMUNEUNION_EVENT_STAT/Z_CAP_C_EDZW_LATEST_PVW_STATUS_PREMIUMEVENT_COMMUNEUNION_MUL.zip'),
    MoWaSFeedReader('de-lhp', 'https://warnung.bund.de/bbk.lhp/hochwassermeldungen.json'),
    MoWaSFeedReader('de-mowas', 'https://warnung.bund.de/bbk.mowas/gefahrendurchsagen.json'),
    CAPFeedReader('dk-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-denmark'),
    CAPFeedReader('dz-meteo', 'https://ametvigilance.meteo.dz/rss/rss_meteo_dz.xml'),
    CAPFeedReader('ec-inamhi', 'https://cap-sources.s3.amazonaws.com/ec-inamhi-es/rss.xml'),
    CAPFeedReader('ee-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-estonia'),
    CAPFeedReader('eg-ema', 'https://cap-sources.s3.amazonaws.com/eg-ema-en/rss.xml'),
    CAPFeedReader('es-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-spain'),
    CAPFeedReader('fi-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-finland'),
    CAPFeedReader('fr-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-france'),
    CAPFeedReader('ga-dgm', 'https://cap-sources.s3.amazonaws.com/ga-dgm-en/rss.xml'),
    CAPFeedReader('gb-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-united-kingdom'),
    CAPFeedReader('gm-dwr', 'https://cap-sources.s3.amazonaws.com/gm-dwr-en/rss.xml'),
    CAPFeedReader('gn-dnm', 'https://cap-sources.s3.amazonaws.com/gn-dnm-en/rss.xml'),
    CAPFeedReader('gr-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-greece'),
    CAPFeedReader('gw-inm', 'https://cap-sources.s3.amazonaws.com/gw-inm-fr/rss.xml'),
    CAPFeedReader('gy-hydromet', 'https://hydromet.gov.gy/cap/en/alerts/rss.xml'),
    CAPFeedReader('hk-weather', 'https://alerts.weather.gov.hk/V1/cap_atom.xml'),
    CAPFeedReader('hr-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-croatia'),
    CAPFeedReader('hu-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-hungary'),
    CAPFeedReader('id-bmkg', 'https://signature.bmkg.go.id/alert/public/en/rss.xml'),
    CAPFeedReader('ie-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-ireland'),
    CAPFeedReader('il-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-israel'),
    CAPFeedReader('in-imd', 'https://cap-sources.s3.amazonaws.com/in-imd-en/rss.xml'),
    CAPFeedReader('ir-irimo', 'https://cap-sources.s3.amazonaws.com/ir-irimo-en/rss.xml'),
    #CAPFeedReader('is-vedur', 'https://api.vedur.is/cap/v1/capbroker/active/category/Met/'), # TODO service not responding
    CAPFeedReader('it-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-italy'),
    CAPFeedReader('ja-meteo', 'https://alert.metservice.gov.jm/capfeed.php'),
    CAPFeedReader('ke-kmd', 'https://cap-sources.s3.amazonaws.com/ke-kmd-en/rss.xml'),
    CAPFeedReader('km-anacm', 'https://cap-sources.s3.amazonaws.com/km-anacm-en/rss.xml'),
    CAPFeedReader('kw-met', 'https://www.met.gov.kw/rss_eng/kuwait_cap.xml'),
    CAPFeedReader('ls-lms', 'https://cap-sources.s3.amazonaws.com/ls-lms-en/rss.xml'),
    CAPFeedReader('lt-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-lithuania'),
    CAPFeedReader('lu-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-luxembourg'),
    CAPFeedReader('lv-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-latvia'),
    CAPFeedReader('md-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-moldova'),
    CAPFeedReader('me-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-montenegro'),
    CAPFeedReader('mg-meteo', 'https://cap-sources.s3.amazonaws.com/mg-meteo-en/rss.xml'),
    CAPFeedReader('mk-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-republic-of-north-macedonia'),
    CAPFeedReader('ml-meteo', 'https://cap-sources.s3.amazonaws.com/ml-meteo-en/rss.xml'),
    #CAPFeedReader('mm-dmh', 'http://www.dmhwarning.gov.mm/eden/cap/public.rss'), # TODO server not responding
    CAPFeedReader('mn-namem', 'https://cap-sources.s3.amazonaws.com/mn-namem-en/rss.xml'),
    CAPFeedReader('mo-smg', 'https://rss.smg.gov.mo/cap_rss.xml'),
    CAPFeedReader('mt-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-malta'),
    CAPFeedReader('mu-mms', 'https://cap-sources.s3.amazonaws.com/mu-mms-en/rss.xml'),
    CAPFeedReader('mw-met', 'https://cap-sources.s3.amazonaws.com/mw-met-en/rss.xml'),
    CAPFeedReader('mx-smn', 'https://smn.conagua.gob.mx/tools/PHP/feedsmn/cap.php'),
    CAPFeedReader('ne-meteo', 'https://cap-sources.s3.amazonaws.com/ne-meteo-en/rss.xml'),
    CAPFeedReader('nl-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-netherlands'),
    CAPFeedReader('no-met', 'https://alert.met.no/alerts'),
    CAPFeedReader('nz-metservice', 'https://alerts.metservice.com/cap/rss'),
    CAPFeedReader('om-met', 'https://cap-sources.s3.amazonaws.com/om-met-en/rss.xml'),
    #CAPFeedReader('pg-pngmet', 'https://smartalert.pngmet.gov.pg/capfeed.php'), # TODO SSL certificate error
    CAPFeedReader('ph-pagasa', 'https://publicalert.pagasa.dost.gov.ph/feeds/'),
    CAPFeedReader('pl-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-poland'),
    CAPFeedReader('pt-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-portugal'),
    CAPFeedReader('py-dmh', 'https://cap-sources.s3.amazonaws.com/py-dmh-en/rss.xml'),
    CAPFeedReader('ro-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-romania'),
    CAPFeedReader('rs-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-serbia'),
    CAPFeedReader('ru-meteoinfo', 'https://meteoinfo.ru/hmc-output/cap/cap-feed/en/atom.xml'),
    #CAPFeedReader('sa-ncm', 'https://ncm.gov.sa/Ar/alert/Pages/feedalerts.aspx'), # TODO no polygons or geo codes?
    #CAPFeedReader('sb-met', 'https://smartalert.met.gov.sb/capfeed.php'), # TODO server no responding
    CAPFeedReader('sc-meteo', 'https://cap-sources.s3.amazonaws.com/sc-meteo-en/rss.xml'),
    CAPFeedReader('se-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-sweden'),
    CAPFeedReader('si-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-slovenia'),
    CAPFeedReader('sn-anacim', 'https://cap-sources.s3.amazonaws.com/sn-anacim-en/rss.xml'),
    CAPFeedReader('sk-meteoalarm', 'https://feeds.meteoalarm.org/feeds/meteoalarm-legacy-atom-slovakia'),
    CAPFeedReader('sr-meteo', 'https://cap-sources.s3.amazonaws.com/sr-meteo-en/rss.xml'),
    CAPFeedReader('sz-met', 'https://cap-sources.s3.amazonaws.com/sz-met-en/rss.xml'),
    CAPFeedReader('tg-dgmn', 'https://cap-sources.s3.amazonaws.com/tg-dgmn-en/rss.xml'),
    #CAPFeedReader('th-tmd', 'https://www.tmd.go.th/feeds/CAPfeeds.php'), # TODO needs ISO-3166-2 boundary polygons
    CAPFeedReader('tl-dnmg', 'https://cap-sources.s3.amazonaws.com/tl-dnmg-en/rss.xml'),
    CAPFeedReader('tn-meteo', 'https://cap-sources.s3.amazonaws.com/tn-meteo-en/rss.xml'),
    #CAPFeedReader('to-met', 'https://alerts.met.gov.to/capfeed.php'), # TODO SSL certificate error
    CAPFeedReader('tt-ttms', 'https://metproducts.gov.tt/ttms/public/api/feed?type=rss'),
    CAPFeedReader('tz-tma', 'https://cap-sources.s3.amazonaws.com/tz-tma-en/rss.xml'),
    CAPFeedReader('ug-unma', 'https://cap-sources.s3.amazonaws.com/ug-unma-en/rss.xml'),
    CAPFeedReader('us-nws', 'https://alerts.weather.gov/cap/us.php?x=0'),
    CAPFeedReader('us-nws-mz', 'https://alerts.weather.gov/cap/mzus.php?x=0'),
    CAPFeedReader('us-ntwc', 'https://www.tsunami.gov/events/xml/PAAQAtom.xml'),
    CAPFeedReader('us-ptwc', 'https://www.tsunami.gov/events/xml/PHEBAtom.xml'),
    CAPFeedReader('vg-bviddm', 'https://cap-sources.s3.amazonaws.com/uk-bviddm-en/rss.xml'),
    CAPFeedReader('vu-met', 'https://smartalert.vmgd.gov.vu/capfeed.php'),
    #CAPFeedReader('ws-samet', 'http://alert.samet.gov.ws/capfeed.php'), # TODO server not respondings
    CAPFeedReader('za-weathersa', 'https://caps.weathersa.co.za/Home/RssFeed'),
    CAPFeedReader('zw-msd', 'https://cap-sources.s3.amazonaws.com/zw-msd-en/rss.xml'),
]

for feedReader in feedReaders:
    if arguments.only == None or arguments.only == feedReader.issuerId:
        feedReader.update()
