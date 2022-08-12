# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import json
import requests
import os
import xml.etree.ElementTree as ET

class AbstractFeedReader:
    aggregatorBaseUrl = 'http://localhost:8000/aggregator/'
    issuerId = ''
    alertIds = []

    def update(self):
        self.alertIds = []
        self.updateFeed()
        requests.post(f"{self.aggregatorBaseUrl}feeder/{self.issuerId}/activeAlerts", json=self.alertIds)
        self.alertIds = []

    def loadCapData(self, capSource, capData):
        if capData:
            return capData
        return requests.get(capSource).content

    def geojsonPolygonToCAP(self, coords):
        poly = ''
        for coord in coords[0]:
            poly += f"{coord[0]},{coord[1]} "
        return poly.strip()

    def expandGeoCodes(self, capData):
        root = ET.fromstring(capData)
        expanded = False
        for area in root.findall('.//{urn:oasis:names:tc:emergency:cap:1.2}area'):
            if area.find('polygon'):
                continue
            for geocode in area.findall('{urn:oasis:names:tc:emergency:cap:1.2}geocode'):
                codeName = geocode.find('{urn:oasis:names:tc:emergency:cap:1.2}valueName').text
                codeValue = geocode.find('{urn:oasis:names:tc:emergency:cap:1.2}value').text
                codeFile = os.path.join('data', codeName, f"{codeValue}.geojson")
                if os.path.isfile(codeFile):
                    geojson = json.load(open(codeFile))
                    if geojson['geometry']['type'] == 'Polygon':
                        poly = ET.SubElement(area, '{urn:oasis:names:tc:emergency:cap:1.2}polygon')
                        poly.text = self.geojsonPolygonToCAP(geojson['geometry']['coordinates'])
                        expanded = True
                    elif geojson['geometry']['type'] == 'MultiPolygon':
                        for coords in geojson['geometry']['coordinates']:
                            poly = ET.SubElement(area, '{urn:oasis:names:tc:emergency:cap:1.2}polygon')
                            poly.text = self.geojsonPolygonToCAP(coords)
                        expanded = True
                    else:
                        print(f"unhandled geometry type: {geojson['geometry']['type']}")
                else:
                    print(f"can't expand code {codeName}: {codeValue}")

        if expanded:
            capData = ET.tostring(root, encoding='utf-8', xml_declaration=True).decode()
        return (capData, expanded)

    def addAlert(self, alertId, issuedTime, capSource = None, capData = None, expireTime = None):
        self.alertIds.append(alertId)

        capData = self.loadCapData(capSource, capData)
        (capData, geoCodesExpanded) = self.expandGeoCodes(capData)

        alert = {}
        alert['issuerId'] = self.issuerId
        alert['alertId'] = alertId
        alert['issueTime'] = issuedTime
        if expireTime:
            alert['expireTime'] = expireTime
        if capSource and not geoCodesExpanded:
            alert['capSource'] = capSource
        elif capData:
            alert['capData'] = capData

        requests.post(f"{self.aggregatorBaseUrl}feeder/{self.issuerId}/alert", json=alert)
