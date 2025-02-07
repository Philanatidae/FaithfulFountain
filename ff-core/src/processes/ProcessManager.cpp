/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ff/processes/Process.hpp"
#include <ff/processes/ProcessManager.hpp>

#include <ff/processes/InstantProcess.hpp>

#include <algorithm>

#include <ff/Console.hpp>

namespace ff {
    ProcessManager::ProcessManager()
        :_isTicking(false) {

    }
    ProcessManager::~ProcessManager() {
        killAll(true);
        _processes.clear();
    }

    std::shared_ptr<Process> ProcessManager::attachProcess(const std::shared_ptr<Process>& process, const int& flags) {
        process->addFlags(flags);

        if(_isTicking) {
            _pendingProcesses.push_back(process);
            return process;
        }

        if(process->getPriority() <= ProcessPriority::INHERITED) {
            // If a process has inherited priority,
            // it was not attached to a process.
            // Change to default.
            process->setPriority(ProcessPriority::DEFAULT);
        }
        _processes.push_back(process);
        std::sort(_processes.begin(), _processes.end(), [](const std::shared_ptr<Process>& lhs, const std::shared_ptr<Process>& rhs) -> bool {
            return lhs->getPriority() > rhs->getPriority();
        });
        _processes.erase(std::unique(_processes.begin(), _processes.end()), _processes.end());

        if(process->hasFlag(ProcessFlags::INITIALIZE_ON_ATTACH)) {
            process->initialize();
        }

        process->_attached = true;

        return process;
    }
    void ProcessManager::tick(const float& dt) { // @todo Rename to `update`, tick doesn't make sense
        _isTicking = true;

        std::vector<std::shared_ptr<Process>> deadProcesses;

        for(size_t i = 0; i < _processes.size(); i++) {
            std::shared_ptr<Process> process = _processes[i];
            process->preUpdate();
        }
        for(size_t i = 0; i < _processes.size(); i++) {
            std::shared_ptr<Process> process = _processes[i];
            if (process->getInitialized()) {
                process->update(dt);
            }
        }

        for(size_t i = 0; i < _processes.size(); i++) {
            std::shared_ptr<Process> process = _processes[i];
            if(process->getInitialized()) {
                process->postUpdate();

                if (!process->getAlive()) {
                    deadProcesses.push_back(process);
                }
            }
        }
        
        _isTicking = false;

        for(size_t i = 0; i < deadProcesses.size(); i++) {
            auto process = deadProcesses[i];

            _processes.erase(std::remove(_processes.begin(), _processes.end(), process), _processes.end());

            if (auto nextProcess = process->getNext()) {
                auto instantProcess = std::dynamic_pointer_cast<InstantProcess>(nextProcess);
                while(nextProcess && instantProcess) {
                    // @todo I don't know why this is being called directly instead of through a proxy function
                    instantProcess->onTrigger();
                    instantProcess->kill();
                    nextProcess = instantProcess->getNext();
                    instantProcess = std::dynamic_pointer_cast<InstantProcess>(nextProcess);
                }
                if(nextProcess) {
                    attachProcess(nextProcess);
                    nextProcess->preUpdate();
                    nextProcess->update(dt);
                    nextProcess->postUpdate();
                }
            }
        }

        if(_pendingProcesses.size() > 0) {
            for(auto it = _pendingProcesses.begin();
                it != _pendingProcesses.end();
                it++) {
                attachProcess(*it);
            }
            _pendingProcesses.clear();
        }
    }
    void ProcessManager::killAll(const bool& immediate, const bool& stopChains) {
        for(auto it = _processes.begin();
            it != _processes.end();
            it++) {
            if(stopChains) {
                (*it)->setNext(nullptr);
            }
            if(immediate) {
                (*it)->kill();
            } else {
                (*it)->softKill();
            }
        }
    }
    void ProcessManager::togglePauseAll(){
        for(auto it = _processes.begin();
            it != _processes.end();
            it++) {
            if(!(*it)->hasFlag(ProcessFlags::REQUEST_NO_PAUSE)) {
                (*it)->togglePause();
            }
        }
    }

    const std::vector<std::shared_ptr<Process>>& ProcessManager::getProcesses() const {
        return _processes;
    }
}
