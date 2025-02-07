/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_VECTOR_HELPER_HPP
#define _FAITHFUL_FOUNTAIN_VECTOR_HELPER_HPP

#include <glm/glm.hpp>

namespace ff {
    namespace VectorHelper {
        inline glm::vec2 rotate(const glm::vec2& vec, const float& cos, const float& sin) {
            return glm::vec2(vec.x * cos - vec.y * sin, vec.x * sin + vec.y * cos);
        }
        inline glm::vec2 rotate(const glm::vec2& vec, const float& angle) {
            return rotate(vec, glm::cos(angle), glm::sin(angle));
        }

        inline glm::vec2 normal(const glm::vec2& vec) {
            return glm::vec2(-vec.y, vec.x);
        }

        inline glm::vec3 up() {
            return glm::vec3(0, 0, 1);
        }

        inline glm::vec3 cameraLocalForward() {
            return glm::vec3(0, 0, -1);
        }
        inline glm::vec3 cameraLocalRight() {
            return glm::vec3(-1, 0, 0);
        }
        inline glm::vec3 cameraLocalUp() {
            return glm::vec3(0, 1, 0);
        }
    }
}

#endif
