/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ANIMATION_CAMERA_ZOOM_ANIMATION_HPP
#define _FAITHFUL_FOUNTAIN_ANIMATION_CAMERA_ZOOM_ANIMATION_HPP

#include <ff/animation/AnimationProcess.hpp>

#include <ff/graphics/OrthographicCamera.hpp>

#include <glm/glm.hpp>

namespace ff {
    class CameraZoomAnimation : public ff::AnimationProcess {
    public:
        CameraZoomAnimation(ff::OrthographicCamera* const& camera,
            const float& duration,
            const float& finalZoom,
            const Easings::Functions& easingFunction = Easings::Functions::LINEAR,
            ProcessPriority_t const& = ff::ProcessPriority::DEFAULT);
        virtual ~CameraZoomAnimation();

        ff::OrthographicCamera* getCamera() const;

    protected:
        void onInitialize() override;
        void onAnimate(const float& dt) override;

    private:
        ff::OrthographicCamera* const _camera;
        float _initialZoom;
        float _finalZoom;
    };
}

#endif
