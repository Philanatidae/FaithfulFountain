/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/math/MatrixHelper.hpp>

namespace ff {
    namespace MatrixHelper {
        glm::mat4x4 createProjection(ClipSpaceHandedness const& hand,
            float const& fovy,
            float const& aspectRatio,
            float const& near,
            float const& far) {
            float const g = 1/(glm::tan(fovy/2));
            float const handMx = hand == ClipSpaceHandedness::RIGHT ? 1 : -1;

            return glm::mat4x4(glm::vec4(g/aspectRatio,
                    0,
                    0,
                    0),
                glm::vec4(0,
                    g,
                    0,
                    0),
                glm::vec4(0,
                    0,
                    handMx * far/(far - near),
                    handMx * 1),
                glm::vec4(0,
                    0,
                    -near*far/(far - near),
                    0));
        }
        glm::mat4x4 createProjectionInfinite(ClipSpaceHandedness const& hand,
            float const& fovy,
            float const& aspectRatio,
            float const& near) {
            float const g = 1/(glm::tan(fovy/2));
            float const e = glm::pow(2.0f, -20.0f);
            float const handMx = hand == ClipSpaceHandedness::RIGHT ? 1 : -1;

            return glm::mat4x4(glm::vec4(g/aspectRatio,
                    0,
                    0,
                    0),
                glm::vec4(0,
                    g,
                    0,
                    0),
                glm::vec4(0,
                    0,
                    handMx * (1 - e),
                    handMx * 1),
                glm::vec4(0,
                    0,
                    -near * (1 - e),
                    0));
        }
        glm::mat4x4 createProjectionInfiniteReverse(ClipSpaceHandedness const& hand,
            float const& fovy,
            float const& aspectRatio,
            float const& near) {
            float const g = 1/(glm::tan(fovy/2));
            float const e = glm::pow(2.0f, -20.0f);
            float const handMx = hand == ClipSpaceHandedness::RIGHT ? 1 : -1;

            return glm::mat4x4(glm::vec4(g/aspectRatio,
                    0,
                    0,
                    0),
                glm::vec4(0,
                    g,
                    0,
                    0),
                glm::vec4(0,
                    0,
                    handMx * e,
                    handMx * 1),
                glm::vec4(0,
                    0,
                    near * (1 - e),
                    0));
        }
    }
}
