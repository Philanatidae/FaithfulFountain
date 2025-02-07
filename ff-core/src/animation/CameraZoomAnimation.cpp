/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/animation/CameraZoomAnimation.hpp>

namespace ff {
    CameraZoomAnimation::CameraZoomAnimation(ff::OrthographicCamera* const& camera,
        const float& duration,
        const float& finalZoom,
        const Easings::Functions& easingFunction,
        ProcessPriority_t const& priority)
        :ff::AnimationProcess(duration, easingFunction, priority),
        _camera(camera),_finalZoom(finalZoom) {
    }
    CameraZoomAnimation::~CameraZoomAnimation() {
    }

    ff::OrthographicCamera* CameraZoomAnimation::getCamera() const {
        return _camera;
    }

    void CameraZoomAnimation::onInitialize() {
        _initialZoom = _camera->getZoom();
    }
    void CameraZoomAnimation::onAnimate(const float& dt) {
        _camera->setZoom(glm::mix(_initialZoom,
            _finalZoom,
            getInterpolatedAlpha()));
    }
}
