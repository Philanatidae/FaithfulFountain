/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_PROCESSES_BUFFER_PROCESS_HPP
#define _FAITHFUL_FOUNTAIN_PROCESSES_BUFFER_PROCESS_HPP

#include <ff/processes/Process.hpp>

namespace ff {
    class BufferProcess final : public Process {
    public:
        BufferProcess();
        ~BufferProcess();

    protected:
        void onInitialize() override;
    };
}

#endif
