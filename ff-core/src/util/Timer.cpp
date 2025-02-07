/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/util/Timer.hpp>

namespace ff {
    Timer::Timer(const float& duration)
        :_duration(duration),_elapsed(0) {
    }
    Timer::~Timer() {
    }

    void Timer::update(const float& dt) {
        _elapsed += dt;
    }

    bool Timer::hasElapsed() const {
        return _elapsed >= _duration;
    }

    void Timer::reset() {
        _elapsed = 0;
    }
    void Timer::reset(const float& newDuration) {
        _duration = newDuration;
        _elapsed = 0;
    }

    float Timer::getAlpha() const {
        return _elapsed / _duration;
    }
    float Timer::getClampedAlpha() const {
        if(_elapsed > _duration) {
            return 1;
        }
        return getAlpha();
    }

    float Timer::getElapsed() const {
        return _elapsed;
    }
    float Timer::getDuration() const {
        return _duration;
    }
}
