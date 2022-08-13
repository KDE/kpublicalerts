# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import datetime
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
        return requests.get(capSource).content.decode('utf-8')

    def geojsonPolygonToCAP(self, coords):
        poly = ''
        for coord in coords[0]:
            poly += f"{coord[1]},{coord[0]} "
        return poly.strip()

    def expandGeoCodes(self, capTree):
        expanded = False
        for area in capTree.findall('.//{urn:oasis:names:tc:emergency:cap:1.2}area'):
            if area.find('{urn:oasis:names:tc:emergency:cap:1.2}polygon'):
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
        return expanded

    def addAlert(self, capSource = None, capData = None):
        if not capSource and not capData:
            print(f"{self.issuerId} - Got no CAP alert message, skipping {alertId}")
            return

        capData = self.loadCapData(capSource, capData)
        capDataModified = False
        # crude way to normalize to CAP v1.2, US NWS still uses v1.1 data
        if 'urn:oasis:names:tc:emergency:cap:1.1' in capData:
            capData = capData.replace('urn:oasis:names:tc:emergency:cap:1.1', 'urn:oasis:names:tc:emergency:cap:1.2')
            capDataModified = True

        ET.register_namespace('', 'urn:oasis:names:tc:emergency:cap:1.2')
        capTree = ET.fromstring(capData)

        # find identifier
        idNode = capTree.find('{urn:oasis:names:tc:emergency:cap:1.2}identifier')
        if idNode == None or not idNode.text:
            print(f"{self.issuerId} - Couldn't find CAP alert message identifier, skipping")
            return
        alertId = idNode.text

        # find sent time
        sentTimeNode = capTree.find('{urn:oasis:names:tc:emergency:cap:1.2}sent')
        if sentTimeNode == None or not sentTimeNode.text:
            print(f"{self.issuerId} - Couldn't find CAP alert message sent time, skipping {alertId}")
        sentTime = sentTimeNode.text

        # find expire time
        expireTimeNode = capTree.find('{urn:oasis:names:tc:emergency:cap:1.2}info/{urn:oasis:names:tc:emergency:cap:1.2}expires')
        expireTime = None
        if expireTimeNode != None:
            expireTime = expireTimeNode.text
            dt = datetime.datetime.fromisoformat(expireTime)
            now = datetime.datetime.now(datetime.timezone.utc)
            if dt < now:
                print(f"{self.issuerId} - skipping alert {alertId} expired on {dt}")
                return

        # expand geocodes if necessary, and determine bounding box
        capDataModified |= self.expandGeoCodes(capTree)
        if capDataModified:
            capData = ET.tostring(capTree, encoding='utf-8', xml_declaration=True).decode()

        # TODO determine bounding box and drop elements without
        hasGeo = False
        for polyNode in capTree.findall('{urn:oasis:names:tc:emergency:cap:1.2}info/{urn:oasis:names:tc:emergency:cap:1.2}area/{urn:oasis:names:tc:emergency:cap:1.2}polygon'):
            if polyNode.text:
                hasGeo = True
                break
        for circleNode in capTree.findall('{urn:oasis:names:tc:emergency:cap:1.2}info/{urn:oasis:names:tc:emergency:cap:1.2}area/{urn:oasis:names:tc:emergency:cap:1.2}circle'):
            if circleNode.text:
                hasGeo = True
                break
        if not hasGeo:
            print(f"{self.issuerId} - No geographic data available for {alertId} - skipping")
            return

        self.alertIds.append(alertId)
        alert = {}
        alert['issuerId'] = self.issuerId
        alert['alertId'] = alertId
        alert['issueTime'] = sentTime
        if expireTime:
            alert['expireTime'] = expireTime
        if capSource and not capDataModified:
            alert['capSource'] = capSource
        elif capData:
            alert['capData'] = capData

        requests.post(f"{self.aggregatorBaseUrl}feeder/{self.issuerId}/alert", json=alert)
