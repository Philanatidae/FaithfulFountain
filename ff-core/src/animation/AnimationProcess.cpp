/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/animation/AnimationProcess.hpp>

#include <ff/Console.hpp>

namespace ff {
    AnimationProcess::AnimationProcess(const float& duration,
        const Easings::Functions& function,
        ProcessPriority_t const& priority)
        :Process(priority),_timer(duration),_function(function) {
    }
    AnimationProcess::~AnimationProcess() {
    }

    const Timer& AnimationProcess::getTimer() const {
        return _timer;
    }

    float AnimationProcess::getAlpha() const {
        return _timer.getAlpha();
    }
    float AnimationProcess::getClampedAlpha() const {
        return _timer.getClampedAlpha();
    }

    float AnimationProcess::getInterpolatedAlpha() const {
        return Easings::interpolate(getEasingFunction(), getClampedAlpha());
    }

    const Easings::Functions& AnimationProcess::getEasingFunction() const {
        return _function;
    }

    void AnimationProcess::onAnimationEnd() {
    }

    void AnimationProcess::onUpdate(const float& dt) {
        _timer.update(dt);
        if(_timer.hasElapsed()) {
            kill();
            return;
        }
        onAnimate(dt);
    }
    void AnimationProcess::onKill() {
        if(!getAlive()) {
            return;
        }
        // This sets alpha to 1, so when the process
        // ends, the process sets the animation to
        // its end state.
        _timer.reset();
        _timer.update(_timer.getDuration());
        onAnimate(0);
        onAnimationEnd();
    }
}
