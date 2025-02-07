/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_PROCESSES_PROCESS_MANAGER_HPP
#define _FAITHFUL_FOUNTAIN_PROCESSES_PROCESS_MANAGER_HPP

#include <ff/processes/Process.hpp>
#include <memory>
#include <vector>

namespace ff {
    class ProcessManager final {
    public:
        ProcessManager();
        virtual ~ProcessManager();

        std::shared_ptr<Process> attachProcess(const std::shared_ptr<Process>& process, const int& flags = 0);
        template<typename P, typename... Args>
        std::shared_ptr<P> attachProcess(Args... args);

        void tick(const float& dt);
        void killAll(const bool& immediate = false, const bool& stopChains = false);
        void togglePauseAll();

        const std::vector<std::shared_ptr<Process>>& getProcesses() const;

    protected:
        std::vector<std::shared_ptr<Process>> _processes;
    private:
        std::vector<std::shared_ptr<Process>> _pendingProcesses;
        bool _isTicking;
    };

    template<typename P, typename... Args>
    std::shared_ptr<P> ProcessManager::attachProcess(Args... args) {
        return std::static_pointer_cast<P>(attachProcess(std::make_shared<P>(args...)));
    }
}

#endif
