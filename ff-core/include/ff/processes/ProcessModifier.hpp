/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_PROCESSES_PROCESS_MODIFIER_HPP
#define _FAITHFUL_FOUNTAIN_PROCESSES_PROCESS_MODIFIER_HPP

namespace ff {
    class Process;

    class ProcessModifier {
    friend class Process;

    public:
        ProcessModifier();
        virtual ~ProcessModifier();

        Process* const& getProcess() const;
        bool isAttached() const;

    protected:
        virtual void onAttach();
        virtual void onInitialize();
        virtual void onUpdate(const float& dt);
        virtual void onKill();
        virtual void onTogglePause();

    private:
        void attach(Process* const& process);

        Process* _process;
    };
}

#include <ff/processes/Process.hpp>

#endif
