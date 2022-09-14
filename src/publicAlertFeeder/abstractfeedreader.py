# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import datetime
import json
import requests
import os
import xml.etree.ElementTree as ET

class AbstractFeedReader:
    aggregatorBaseUrl = f"http://{os.environ.get('AGGREGATOR_HOST', '127.0.0.1')}:8000/feeder/"
    issuerId = ''
    alertIds = []

    def update(self):
        self.alertIds = []
        self.updateFeed()
        try:
            requests.post(f"{self.aggregatorBaseUrl}feeder/{self.issuerId}/activeAlerts", json=self.alertIds)
        except requests.exceptions.ConnectionError as e:
            print(f"Could not connect to aggregator")
        self.alertIds = []

    def geojsonPolygonToCAP(self, coords):
        poly = ''
        for coord in coords[0]:
            poly += f"{coord[1]:.4f},{coord[0]:.4f} "
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

    def bboxForPoly(poly, minlat, minlon, maxlat, maxlon):
        if not poly:
            return (minlat, minlon, maxlat, maxlon)
        for point in poly.split(' '):
            coords = point.split(',')
            if len(coords) != 2:
                continue
            try:
                 lat = float(coords[0])
                 lon = float(coords[1])
            except ValueError:
                continue
            minlat = min(minlat, lat)
            maxlat = max(maxlat, lat)
            minlon = min(minlon, lon)
            maxlon = max(maxlon, lon)
        return (minlat, minlon, maxlat, maxlon)

    def bboxForCircle(circle, minlat, minlon, maxlat, maxlon):
        (center, radius) = circle.split(' ')
        (lat, lon) = center.split(',')
        try:
            lat = float(lat)
            lon = float(lon)
        except ValueError:
            return (minlat, minlon, maxlat, maxlon)

        # TODO correct radius computation
        minlat = min(minlat, lat - 1.0)
        maxlat = max(maxlat, lat + 1.0)
        minlon = min(minlon, lon - 1.0)
        maxlon = max(maxlon, lon + 1.0)
        return (minlat, minlon, maxlat, maxlon)

    def flattenXml(node):
        node.tail = None
        if node.text != None:
            node.text = node.text.strip()
        for child in node:
            AbstractFeedReader.flattenXml(child)

    def addAlert(self, capSource = None, capData = None):
        if not capData:
            print(f"{self.issuerId} - Got no CAP alert message, skipping")
            return

        capDataModified = False
        # crude way to normalize to CAP v1.2, US NWS still uses v1.1 data
        if 'urn:oasis:names:tc:emergency:cap:1.1' in capData:
            capData = capData.replace('urn:oasis:names:tc:emergency:cap:1.1', 'urn:oasis:names:tc:emergency:cap:1.2')
            capDataModified = True

        ET.register_namespace('', 'urn:oasis:names:tc:emergency:cap:1.2')
        try:
            capTree = ET.fromstring(capData)
        except ET.ParseError as e:
            print(f"{self.issuerId} - failed to parse CAP alert message XML: {e}")
            print(capData)
            return

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
        expireTime = None
        for expireTimeNode in capTree.findall('{urn:oasis:names:tc:emergency:cap:1.2}info/{urn:oasis:names:tc:emergency:cap:1.2}expires'):
            dt = datetime.datetime.fromisoformat(expireTimeNode.text)
            if expireTime == None or dt > expireTime:
                expireTime = dt
        if expireTime != None and expireTime < datetime.datetime.now(datetime.timezone.utc):
            print(f"{self.issuerId} - skipping alert {alertId} expired on {dt}")
            return

        # expand geocodes if necessary, and determine bounding box
        capDataModified |= self.expandGeoCodes(capTree)
        if capDataModified or capSource == None:
            AbstractFeedReader.flattenXml(capTree)
            capData = ET.tostring(capTree, encoding='utf-8', xml_declaration=True).decode()

        # determine bounding box and drop elements without
        minlat = 90
        minlon = 180
        maxlat = -90
        maxlon = -180
        for polyNode in capTree.findall('{urn:oasis:names:tc:emergency:cap:1.2}info/{urn:oasis:names:tc:emergency:cap:1.2}area/{urn:oasis:names:tc:emergency:cap:1.2}polygon'):
            (minlat, minlon, maxlat, maxlon) = AbstractFeedReader.bboxForPoly(polyNode.text, minlat, minlon, maxlat, maxlon)
        for circleNode in capTree.findall('{urn:oasis:names:tc:emergency:cap:1.2}info/{urn:oasis:names:tc:emergency:cap:1.2}area/{urn:oasis:names:tc:emergency:cap:1.2}circle'):
            (minlat, minlon, maxlat, maxlon) = AbstractFeedReader.bboxForCircle(circleNode.text, minlat, minlon, maxlat, maxlon)
        if minlat > maxlat or minlon > maxlon:
            print(f"{self.issuerId} - No geographic data available for {alertId} - skipping")
            return

        self.alertIds.append(alertId)
        alert = {}
        alert['issuerId'] = self.issuerId
        alert['alertId'] = alertId
        alert['issueTime'] = sentTime
        alert['minlat'] = minlat
        alert['minlon'] = minlon
        alert['maxlat'] = maxlat
        alert['maxlon'] = maxlon
        if expireTime:
            alert['expireTime'] = expireTime.isoformat()
        if capSource and not capDataModified:
            alert['capSource'] = capSource
        elif capData:
            alert['capData'] = capData

        try:
            req = requests.post(f"{self.aggregatorBaseUrl}feeder/{self.issuerId}/alert", json=alert)
            if not req.ok:
                print(f"Failed to post alert: {req.status_code}")
        except requests.exceptions.ConnectionError as e:
            print(f"Could not connect to aggregator")
