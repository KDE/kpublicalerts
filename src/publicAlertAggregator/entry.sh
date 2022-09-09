#!/bin/bash
# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: CC0-1.0

python3 manage.py collectstatic --clear --no-input
python3 manage.py migrate

# TODO
# CMD uwsgi --http :8000 --wsgi-file /app/publicAlertAggregator/wsgi.py
python3 manage.py runserver 0.0.0.0:8000
