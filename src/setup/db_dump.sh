#!/bin/sh
# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: CC0-1.0

docker-compose exec db pg_dump -U postgres -c -C --inserts kpublicalerts
