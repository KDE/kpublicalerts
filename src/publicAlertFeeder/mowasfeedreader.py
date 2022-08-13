# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import json
import requests
import xml.etree.ElementTree as ET

from abstractfeedreader import AbstractFeedReader

class MoWaSFeedReader(AbstractFeedReader):
    feedUrl = 'https://warnung.bund.de/bbk.mowas/gefahrendurchsagen.json'

    def __init__(self):
        self.issuerId = 'de-mowas'

    def convertProperty(xmlParent, mowasObj, propertyName):
        if not propertyName in mowasObj:
            return
        node = ET.SubElement(xmlParent, '{urn:oasis:names:tc:emergency:cap:1.2}' + propertyName)
        node.text = mowasObj[propertyName];

    def updateFeed(self):
        req = requests.get(self.feedUrl)
        feedData = json.loads(req.content)
        for alert in feedData:
            expireTime = None
            ET.register_namespace('', 'urn:oasis:names:tc:emergency:cap:1.2')
            root = ET.Element('{urn:oasis:names:tc:emergency:cap:1.2}alert')
            for prop in ['identifier', 'sender', 'sent', 'status', 'msgType', 'scope', 'note', 'references']:
                MoWaSFeedReader.convertProperty(root, alert, prop)

            for alertInfo in alert['info']:
                if 'expires' in alertInfo:
                    expireTime = alertInfo['expires']

                info = ET.SubElement(root, '{urn:oasis:names:tc:emergency:cap:1.2}info', {'lang': alertInfo['language']})
                for category in alertInfo['category']:
                    cat = ET.SubElement(info, '{urn:oasis:names:tc:emergency:cap:1.2}category')
                    cat.text = category

                for prop in ['event', 'urgency', 'severity', 'certainty', 'headline', 'description', 'instruction', 'expires', 'web', 'contact' ]:
                    MoWaSFeedReader.convertProperty(info, alertInfo, prop)

                for eventCode in alertInfo['eventCode']:
                    eventCodeNode = ET.SubElement(info, '{urn:oasis:names:tc:emergency:cap:1.2}eventCode')
                    for prop in ['valueName', 'value' ]:
                        MoWaSFeedReader.convertProperty(eventCodeNode, eventCode, prop)

                for param in alertInfo['parameter']:
                    paramNode = ET.SubElement(info, '{urn:oasis:names:tc:emergency:cap:1.2}parameter')
                    for prop in ['valueName', 'value' ]:
                        MoWaSFeedReader.convertProperty(paramNode, param, prop)
                    if param['valueName'] == 'sender_signature':
                        senderNameNode = ET.SubElement(info, '{urn:oasis:names:tc:emergency:cap:1.2}senderName')
                        senderNameNode.text = param['value']

                for area in alertInfo['area']:
                    areaNode = ET.SubElement(info, '{urn:oasis:names:tc:emergency:cap:1.2}area')
                    MoWaSFeedReader.convertProperty(areaNode, area, 'areaDesc')
                    for poly in area['polygon']:
                        polyNode = ET.SubElement(areaNode, '{urn:oasis:names:tc:emergency:cap:1.2}polygon')
                        polyNode.text = poly
                    for code in area['geocode']:
                        codeNode = ET.SubElement(info, '{urn:oasis:names:tc:emergency:cap:1.2}geocode')
                        for prop in ['valueName', 'value' ]:
                            MoWaSFeedReader.convertProperty(codeNode, code, prop)
                    # TODO more area content

            self.addAlert(
                alertId = alert['identifier'],
                issuedTime = alert['sent'],
                expireTime = expireTime,
                capData = ET.tostring(root, encoding='utf-8', xml_declaration=True).decode()
            )
