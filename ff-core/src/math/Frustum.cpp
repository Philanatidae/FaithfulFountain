/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/math/Frustum.hpp>

#include <ff/math/MatrixHelper.hpp>

#include <ff/Locator.hpp>

namespace ff {

Frustum::Frustum() {
}
Frustum::Frustum(float const& fov,
    float const& aspectRatio,
    float const& near,
    std::optional<float> const& far) {
    set(fov, aspectRatio, near, far);
}

void Frustum::set(float const& fovy,
    float const& aspectRatio,
    float const& near,
    std::optional<float> const& far) {
    FF_CONSOLE_FATAL("Not implemented.");

    float const fovx = 2*glm::atan(aspectRatio * glm::tan(fovy/2));
    
    // glm::vec3 ntl(glm::sin());




    // _top.normal = glm::vec3();
    // _bottom.normal = glm::vec3(bottom.x, bottom.y, bottom.z);

    // _left.normal = glm::vec3(left.x, left.y, left.z);
    // _right.normal = glm::vec3(right.x, right.y, right.z);

    // _near.normal = glm::vec3(near.x, near.y, near.z);
    // _near.distance = near;

    // if(far) {
    //     _far = Plane(, far.y, far.z),
    //         far.value);
    // } else {
    //     _far = std::nullopt;
    // }
}

}
