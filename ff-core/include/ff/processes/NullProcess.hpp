/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_PROCESSES_NULL_PROCESS_HPP
#define _FAITHFUL_FOUNTAIN_PROCESSES_NULL_PROCESS_HPP

#include <ff/processes/Process.hpp>

namespace ff {
    class NullProcess : public Process {
    public:
        NullProcess() {
        }
        virtual ~NullProcess() {
        }
    };
}

#endif
