/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/PerspectSceneCamera.hpp>

#include <ff/Locator.hpp>

namespace ff {

PerspectSceneCamera::PerspectSceneCamera(float const& fovy,
    float const& near,
    bool const& generateUniformBuffer)
    :PerspectiveCamera(fovy,
        near,
        generateUniformBuffer) {
    Locator::getMessageBus().addListener<ResizeEvent>(this);
}

bool PerspectSceneCamera::processEvent(ff::ResizeEvent const& evt) {
    adjustFrustums(evt.width, evt.height);
    update();

    return false;
}

void PerspectSceneCamera::adjustFrustums(const float& renderTargetWidth,
    const float& renderTargetHeight) {
    // @todo Frustums, etc.

    float const aspectRatio = renderTargetWidth / renderTargetHeight;
    adjust(getFOVY(),
        getNear(),
        aspectRatio);
}

}
