#pragma once

#include <cmath>

namespace grvx::testing {
[[nodiscard]] inline double
great_circle_distance(double lat1, double lon1, double lat2, double lon2)
{
    double s1 = std::sin(lat1);
    double s2 = std::sin(lat2);
    double c1 = std::cos(lat1);
    double c2 = std::cos(lat2);
    double d = std::cos(lon1 - lon2);
    return std::acos(s1 * s2 + c1 * c2 * d);
}
} // namespace grvx::testing