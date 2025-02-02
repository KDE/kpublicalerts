#! /usr/bin/env bash
# SPDX-FileCopyrightText: 2025 Emir SARI <emir_sari@icloud.com>
# SPDX-License-Identifier: CC0-1.0
$XGETTEXT `find -name \*.cpp -o -name \*.qml` -o $podir/kpublicalerts.pot
