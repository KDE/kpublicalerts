# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

from mowasfeedreader import MoWaSFeedReader

feedReaders = [ MoWaSFeedReader() ]

for feedReader in feedReaders:
    feedReader.updateFeed()
