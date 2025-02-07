/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_PROCESSES_DELEGATE_PROCESS_HPP
#define _FAITHFUL_FOUNTAIN_PROCESSES_DELEGATE_PROCESS_HPP

#include <ff/processes/InstantProcess.hpp>

#include <functional>

namespace ff {
    class DelegateProcess: public InstantProcess {
    public:
        DelegateProcess(const std::function<void()>& function);
        ~DelegateProcess();

    protected:
        void onTrigger() override;

    private:
        std::function<void()> _function;
    };
}

#endif
