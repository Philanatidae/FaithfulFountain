/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_PROCESS_WAIT_PROCESS_HPP
#define _FAITHFUL_FOUNTAIN_PROCESS_WAIT_PROCESS_HPP

#include <ff/processes/Process.hpp>
#include <ff/util/Timer.hpp>

namespace ff {
    class WaitProcess: public Process {
    public:
        WaitProcess(const float& duration);
        ~WaitProcess();

        const Timer& getTimer() const;

    private:
        Timer _timer;

        void onUpdate(const float& dt) override;
    };
}

#endif
