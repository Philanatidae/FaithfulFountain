/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_ORTHO_SCENE_CAMERA_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_ORTHO_SCENE_CAMERA_HPP

#include <ff/graphics/OrthographicCamera.hpp>

#include <ff/events/ResizeEvent.hpp>
#include <ff/messages/EventListener.hpp>

namespace ff {
    class OrthoSceneCamera : public OrthographicCamera,
        public ff::EventListener<ff::ResizeEvent> {
    public:
        OrthoSceneCamera(const bool& generateUniformBuffer = true);
        virtual ~OrthoSceneCamera() = default;

        Rectangle const& getViewport() const;

    protected:
        bool processEvent(ff::ResizeEvent const& evt) override;

    private:
        void adjustViewports(const float& renderTargetWidth, const float& renderTargetHeight);

        Rectangle _viewport;
    };
}

#endif
