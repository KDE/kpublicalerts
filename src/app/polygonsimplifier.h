/*
    SPDX-FileCopyrightText: 2021-2022 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPUBLICALERTS_POLYGONSIMPLIFIER_H
#define KPUBLICALERTS_POLYGONSIMPLIFIER_H

#include <KWeatherCore/CAPArea>

namespace KPublicAlerts {

/** Methods to simplify polygon geometry.
 *  (Taken from KPublicTransport)
 */
namespace PolygonSimplifier
{
    /** Douglas Peucker algorithm.
     *  @param distance Threshold in meters.
     *  @see https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm
     */
    KWeatherCore::CAPPolygon douglasPeucker(const KWeatherCore::CAPPolygon &poly, double distance);
}

}

#endif // KPUBLICALERTS_POLYGONSIMPLIFIER_H
