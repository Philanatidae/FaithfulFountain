/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_PROCESSES_RENDER_PROCESS_HPP
#define _FAITHFUL_FOUNTAIN_PROCESSES_RENDER_PROCESS_HPP

#include <ff/processes/Process.hpp>
#include <ff/util/Macros.hpp>

namespace ff {
    class RenderProcess : public Process {
    friend class Game;

    public:
        inline RenderProcess(ProcessPriority_t const& priority = ProcessPriority::INHERITED)
            :Process(priority) {
        }
        inline virtual ~RenderProcess() {
        }

    protected:
        virtual void onRender(const float& dt, const float& alpha) = 0;
    };
}

#endif
