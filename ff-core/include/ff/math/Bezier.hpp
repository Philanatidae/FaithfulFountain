/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MATH_BEZIER_HPP
#define _FAITHFUL_FOUNTAIN_MATH_BEZIER_HPP

#include <glm/glm.hpp>

namespace ff {
namespace Bezier {

inline glm::vec2 calcSecondOrder(glm::vec2 const& p0, glm::vec2 const& p1, glm::vec2 const& p2, float const& t) {
    return (1-t)*(1-t)*p0 + 2*t*(1-t)*p1 + t*t*p2;
}
inline glm::vec3 calcSecondOrder(glm::vec3 const& p0, glm::vec3 const& p1, glm::vec3 const& p2, float const& t) {
    return (1-t)*(1-t)*p0 + 2*t*(1-t)*p1 + t*t*p2;
}
inline glm::vec4 calcSecondOrder(glm::vec4 const& p0, glm::vec4 const& p1, glm::vec4 const& p2, float const& t) {
    return (1-t)*(1-t)*p0 + 2*t*(1-t)*p1 + t*t*p2;
}

inline glm::vec2 calcThirdOrder(glm::vec2 const& p0, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec2 const& p3, float const& t) {
    return (1-t)*(1-t)*(1-t)*p0 + 3*t*(1-t)*(1-t)*p1 + 3*t*t*(1-t)*p2 + t*t*t*p3;
}
inline glm::vec3 calcThirdOrder(glm::vec3 const& p0, glm::vec3 const& p1, glm::vec3 const& p2, glm::vec3 const& p3, float const& t) {
    return (1-t)*(1-t)*(1-t)*p0 + 3*t*(1-t)*(1-t)*p1 + 3*t*t*(1-t)*p2 + t*t*t*p3;
}
inline glm::vec4 calcThirdOrder(glm::vec4 const& p0, glm::vec4 const& p1, glm::vec4 const& p2, glm::vec4 const& p3, float const& t) {
    return (1-t)*(1-t)*(1-t)*p0 + 3*t*(1-t)*(1-t)*p1 + 3*t*t*(1-t)*p2 + t*t*t*p3;
}

}
}

#endif
