/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MATH_PLANE_HPP
#define _FAITHFUL_FOUNTAIN_MATH_PLANE_HPP

#include <glm/glm.hpp>

namespace ff {

struct Plane {
Plane(glm::vec3 const& normal = glm::vec3(0, 0, 0),
    float const& distance = 0);
virtual ~Plane() = default;

glm::vec3 normal;
float distance;

bool isPointBeyondPlane(glm::vec3 const& p);
};

}

#endif
