/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_UTIL_TIMER_HPP
#define _FAITHFUL_FOUNTAIN_UTIL_TIMER_HPP

namespace ff {
    class Timer {
    public:
        Timer(const float& duration);
        virtual ~Timer();

        void update(const float& dt);

        bool hasElapsed() const;

        void reset();
        void reset(const float& newDuration);

        float getAlpha() const;
        float getClampedAlpha() const;

        float getElapsed() const;
        float getDuration() const;
    private:
        float _duration;
        float _elapsed;
    };
}

#endif
