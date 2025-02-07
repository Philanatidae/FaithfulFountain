/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/OrthoSceneCamera.hpp>

#include <ff/CVars.hpp>
#include <ff/Locator.hpp>

namespace ff {
    OrthoSceneCamera::OrthoSceneCamera(const bool& generateUniformBuffer)
        :OrthographicCamera(0, 0, generateUniformBuffer),
        _viewport(0, 0) {
        Locator::getMessageBus().addListener<ResizeEvent>(this);

        adjustViewports(Locator::getGraphicsDevice().getBackBufferDimensions().x,
            Locator::getGraphicsDevice().getBackBufferDimensions().y);
        update();
    }

    Rectangle const& OrthoSceneCamera::getViewport() const {
        return _viewport;
    }

    bool OrthoSceneCamera::processEvent(ff::ResizeEvent const& evt) {
        adjustViewports(evt.width, evt.height);
        update();

        return false;
    }

    void OrthoSceneCamera::adjustViewports(const float& renderTargetWidth, const float& renderTargetHeight) {
        // Our viewport is defined by CVars; the viewport units will not change.
        float viewportHeight = CVars::get<float>("view_viewport_height");
        float viewportWidth = viewportHeight * CVars::get<float>("view_viewport_aspect_ratio");
        const float& viewportAspectRatio = CVars::get<float>("view_viewport_aspect_ratio");

        // We need to now calculate the render area/ortho view bounds, such that:
        // ~ Viewport center is at (0, 0)
        // ~ Render area aspect ratio = render target aspect ratio
        // ~ Viewport + safe area = render area

        const float usableAreaAspectRatio = (renderTargetWidth * (1 - CVars::get<float>("view_safe_area_padding_left") - CVars::get<float>("view_safe_area_padding_right")))
            / (renderTargetHeight * (1 - CVars::get<float>("view_safe_area_padding_top") - CVars::get<float>("view_safe_area_padding_bottom")));

        // The viewport will fill the usable area. Based on the aspect ratio of the usable
        // area, we can find the most restrictive axis, and that axis units will be the same
        // as the viewport on that axis. The other axis units can be found with the aspect
        // ratio.
        float usableAreaWidth = -1,
            usableAreaHeight = -1;
        if(usableAreaAspectRatio > viewportAspectRatio
            || CVars::get<bool>("view_viewport_fill_safe_area")) { // Vertical is the restricting axis, or we are filling the viewport
            usableAreaWidth = viewportWidth;
            usableAreaHeight = usableAreaWidth / usableAreaAspectRatio;
        } else {  // Horizontal is the restricting axis
            usableAreaHeight = viewportHeight;
            usableAreaWidth = usableAreaHeight * usableAreaAspectRatio;
        }

        if(CVars::get<bool>("view_viewport_fill_safe_area")) {
            viewportHeight = usableAreaHeight;
            viewportWidth = usableAreaWidth;
        }

        // Viewport is now properly defined and ready to be used
        _viewport = Rectangle(viewportWidth, viewportHeight);

        // We can now calculate the render area dimensions
        const float renderAreaWidth = usableAreaWidth / (1 - CVars::get<float>("view_safe_area_padding_left") - CVars::get<float>("view_safe_area_padding_right"));
        const float renderAreaHeight = usableAreaHeight / (1 - CVars::get<float>("view_safe_area_padding_top") - CVars::get<float>("view_safe_area_padding_bottom"));

        // We now can know the safe area padding in render area units
        const float safeAreaLeft = renderAreaWidth * CVars::get<float>("view_safe_area_padding_left");
        const float safeAreaRight = renderAreaWidth * CVars::get<float>("view_safe_area_padding_right");
        const float safeAreaTop = renderAreaHeight * CVars::get<float>("view_safe_area_padding_top");
        const float safeAreaBottom = renderAreaHeight * CVars::get<float>("view_safe_area_padding_bottom");

        // We can now start to construct the ortho projection
        const float left = -(usableAreaWidth / 2 + safeAreaLeft);
        const float right = usableAreaWidth / 2 + safeAreaRight;
        const float top = usableAreaHeight / 2 + safeAreaTop;
        const float bottom = -(usableAreaHeight / 2 + safeAreaBottom);

        // Update the ortho project (ortho projection = render area)
        adjust(left, right, top, bottom);
    }
}
