/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/processes/WaitProcess.hpp>

namespace ff {
    WaitProcess::WaitProcess(const float& duration)
        :_timer(duration) {
    }
    WaitProcess::~WaitProcess() {
    }

    const Timer& WaitProcess::getTimer() const {
        return _timer;
    }

    void WaitProcess::onUpdate(const float& dt) {
        _timer.update(dt);
        if(_timer.hasElapsed()) {
            kill();
        }
    }
}
