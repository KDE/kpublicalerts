# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import datetime
import io
import requests
import zipfile

from abstractfeedreader import AbstractFeedReader

class DWDFeedReader(AbstractFeedReader):
    feedUrl = ''
    session = None

    def __init__(self, issuerId, feedUrl):
        self.issuerId = issuerId
        self.feedUrl = feedUrl

    def updateFeed(self):
        feedData = requests.get(self.feedUrl)
        zipFile = zipfile.ZipFile(io.BytesIO(feedData.content), 'r')
        for capFile in zipFile.namelist():
            capData = zipFile.read(capFile).decode('utf-8')
            self.addAlert(capData = capData)
