/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MATH_MATRIX_HELPER_HPP
#define _FAITHFUL_FOUNTAIN_MATH_MATRIX_HELPER_HPP

#include <glm/glm.hpp>
#include <ff/graphics/GPUInfo.hpp>

namespace ff {
    namespace MatrixHelper {
        glm::mat4x4 createProjection(ClipSpaceHandedness const& hand,
            float const& fovy,
            float const& aspectRatio,
            float const& near,
            float const& far);
        glm::mat4x4 createProjectionInfinite(ClipSpaceHandedness const& hand,
            float const& fovy,
            float const& aspectRatio,
            float const& near);
        glm::mat4x4 createProjectionInfiniteReverse(ClipSpaceHandedness const& hand,
            float const& fovy,
            float const& aspectRatio,
            float const& near);
    }
}

#endif
