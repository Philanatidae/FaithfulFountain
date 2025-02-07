/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/math/Plane.hpp>

namespace ff {
Plane::Plane(glm::vec3 const& normal,
    float const& distance)
    :normal(normal),distance(distance) {
}

bool Plane::isPointBeyondPlane(glm::vec3 const& p) {
    return glm::dot(normal, p) - distance;
}
}
