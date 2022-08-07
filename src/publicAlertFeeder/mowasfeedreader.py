# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import json
import requests
import xml.etree.ElementTree as ET

class MoWaSFeedReader:
    feedUrl = 'https://warnung.bund.de/bbk.mowas/gefahrendurchsagen.json'

    def convertProperty(xmlParent, mowasObj, propertyName):
        if not propertyName in mowasObj:
            return
        node = ET.SubElement(xmlParent, '{urn:oasis:names:tc:emergency:cap:1.2}' + propertyName)
        node.text = mowasObj[propertyName];

    def updateFeed(self):
        req = requests.get(self.feedUrl)
        feedData = json.loads(req.content)
        for alert in feedData:
            ET.register_namespace('', 'urn:oasis:names:tc:emergency:cap:1.2')
            root = ET.Element('{urn:oasis:names:tc:emergency:cap:1.2}alerts')
            for prop in ['identifier', 'sender', 'sent', 'status', 'msgType', 'scope']:
                MoWaSFeedReader.convertProperty(root, alert, prop)
            # TODO references: relevant for updates?

            for alertInfo in alert['info']:
                info = ET.SubElement(root, '{urn:oasis:names:tc:emergency:cap:1.2}info', {'lang': alertInfo['language']})
                # TODO category 1 vs N?
                for prop in ['urgency', 'severity', 'certainty', 'headline', 'description', 'instruction']:
                    MoWaSFeedReader.convertProperty(info, alertInfo, prop)
                # TODO eventCode
                # TODO parameter
                # TODO area

            print(ET.tostring(root, encoding='utf-8', xml_declaration=True).decode())

        print("TODO")
