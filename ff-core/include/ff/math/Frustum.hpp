/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MATH_FRUSTRUM_HPP
#define _FAITHFUL_FOUNTAIN_MATH_FRUSTRUM_HPP

#include <glm/glm.hpp>
#include <array>
#include <ff/math/Plane.hpp>
#include <optional>
#include <ff/graphics/GPUInfo.hpp>

namespace ff {

enum class FrustumPlane {
    LEFT = 0,
    RIGHT,
    TOP,
    BOTTOM,
    NEAR,
    FAR,

    COUNT
};

class Frustum {
public:
    Frustum();
    Frustum(float const& fovy,
        float const& aspectRatio,
        float const& near,
        std::optional<float> const& far = std::nullopt);

    void set(float const& fov,
        float const& aspectRatio,
        float const& near,
        std::optional<float> const& far = std::nullopt);

    // Frustum culling needs more work before it is stable. This frustum
    // doesn't have position info, so Viewport and Safe Area are not
    // necessarily where they are supposed to be (they are not required
    // to be centered in clip-space). Render Area is guaranteed to be
    // centered at <0, 0> in clip-space.
    
private:
    Plane _left;
    Plane _right;
    Plane _top;
    Plane _bottom;
    Plane _near;
    std::optional<Plane> _far;
};

}

#endif
