/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _BRICK_GAME_ANIMATION_ANIMATION_PROCESS_HPP
#define _BRICK_GAME_ANIMATION_ANIMATION_PROCESS_HPP

#include <ff/processes/Process.hpp>

#include <ff/util/Timer.hpp>
#include <ff/math/Easings.hpp>

namespace ff {
    class AnimationProcess : public Process {
    public:
        AnimationProcess(const float& duration,
            const Easings::Functions& function = Easings::Functions::LINEAR,
            ProcessPriority_t const& priority = ff::ProcessPriority::DEFAULT);
        virtual ~AnimationProcess();

        const Timer& getTimer() const;

        float getAlpha() const;
        float getClampedAlpha() const;

        float getInterpolatedAlpha() const;

        const Easings::Functions& getEasingFunction() const;

    protected:
        virtual void onAnimate(const float& dt) = 0;
        virtual void onAnimationEnd();

    private:
        void onUpdate(const float& dt) override;
        void onKill() override;

        Timer _timer;
        Easings::Functions _function;
    };
}

#endif
