# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import argparse
import csv
import json
import os

# Get Meteoalarm geocodes.json and geocodes-aliases.csv from https://edrop.zamg.ac.at/owncloud/index.php/s/FegwJnoC3XiesnE

parser = argparse.ArgumentParser(description='Expand Meteoalarm geocodes data.')
parser.add_argument('--output', type=str, required=True, help='Path to which the output should be written to')
arguments = parser.parse_args()

os.makedirs(os.path.join(arguments.output, 'EMMA_ID'), exist_ok=True)

include_countries = ['AT', 'FR']
include_aliases = [ 'FIPS', 'NUTS3' ]

geocodesFile = open('geocodes.json')
geocodes = json.loads(geocodesFile.read())
includedCodes = []
for feature in geocodes['features']:
    if feature['properties']['country'] not in include_countries:
        continue
    if feature['properties']['type'] != 'EMMA_ID':
        print(f"unknown code type: {feature['properties']['type']} - skipping")
        continue
    code = feature['properties']['code']
    codeFile = open(os.path.join(arguments.output, 'EMMA_ID', f"{code}.geojson"), 'w')
    codeFile.write(json.dumps(feature))
    codeFile.close()
    includedCodes.append(code)

with open('geocodes-aliases.csv', newline='') as f:
    geocodesAliases = list(csv.reader(f, delimiter=',', quotechar='"'))[1:]
for alias in geocodesAliases:
    if alias[2] not in include_aliases:
        continue
    if alias[0] in includedCodes:
        destDir = os.path.join(arguments.output, alias[2])
        os.makedirs(os.path.join(destDir), exist_ok=True)
        dest = os.path.join(destDir, f"{alias[1]}.geojson")

        if os.path.isfile(dest):
            os.remove(dest)

        srcDir = os.path.join(arguments.output, 'EMMA_ID')
        src = os.path.join(os.path.relpath(srcDir, destDir), f"{alias[0]}.geojson")
        os.symlink(src, dest)
