/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_PERSPECT_SCENE_CAMERA_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_PERSPECT_SCENE_CAMERA_HPP

#include <ff/graphics/PerspectiveCamera.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff/events/ResizeEvent.hpp>

namespace ff {

class PerspectSceneCamera : public PerspectiveCamera,
    public EventListener<ff::ResizeEvent> {
public:
    PerspectSceneCamera(float const& fovy,
        float const& near,
        bool const& generateUniformBuffer = true);
    virtual ~PerspectSceneCamera() = default;

    // @todo getViewport as Frustum
    // @todo getSafeArea as Frustum

    bool processEvent(ff::ResizeEvent const& evt) override;

private:


    void adjustFrustums(const float& renderTargetWidth,
        const float& renderTargetHeight);
    
    // @todo Frustrum
    // How are safe areas going to be handed? In Ortho, we have three regions: render area (entire render buffer), safe area (guranteed to not be obscured by cutout or edge of screen), and viewport (target area for gameplay/UI). Typically, viewport is close to the safe area, this is mostly used for handling ultra-wide monitors.
    // Naturally, the projection matrix will be based off of the render area. Any projection matrix can be represented by a frustum, so that is automatically taken care of.
    // The safe area and viewport solutions will be extremely similar. Calculate each the same way OrthoSceneCamera does it. Project window-space coordinates at the render plane (which I believe is equivalent to the near plane). A frustum can then be calculated for each (frustum origin is the same for all three, near plane is at the same distance from the origin, the FOV angle can be calculated based on the sides of the safe area/viewport plane, aspect ratio is calculated for free when calculating safe area/viewport.
    // These frustums can then be used to see if anything is in the viewport at any distance by finding the plane at a certain distance.
};

}

#endif
