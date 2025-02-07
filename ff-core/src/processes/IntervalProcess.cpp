/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/processes/IntervalProcess.hpp>

namespace ff {
    IntervalProcess::IntervalProcess(const float& interval)
        :_timer(interval) {
    }
    IntervalProcess::~IntervalProcess() {
    }

    void IntervalProcess::onUpdate(const float& dt) {
        _timer.update(dt);
        if(_timer.hasElapsed()) {
            onTick(_timer.getElapsed());
            _timer.reset();
        }
    }
}
