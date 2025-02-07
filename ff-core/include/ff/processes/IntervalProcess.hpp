/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_PROCESSES_INTERVAL_PROCESS_HPP
#define _FAITHFUL_FOUNTAIN_PROCESSES_INTERVAL_PROCESS_HPP

#include <ff/processes/Process.hpp>
#include <ff/util/Timer.hpp>

namespace ff {
    class IntervalProcess: public Process {
    public:
        IntervalProcess(const float& interval);
        ~IntervalProcess();

    protected:
        virtual void onTick(const float& dt) = 0;

    private:
        Timer _timer;

        void onUpdate(const float& dt) override;
    };
}

#endif
