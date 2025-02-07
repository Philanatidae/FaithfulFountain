/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/processes/ProcessModifier.hpp>

#include <ff/processes/Process.hpp>

namespace ff {
    ProcessModifier::ProcessModifier()
        :_process(nullptr) {
    }
    ProcessModifier::~ProcessModifier() {
    }

    Process* const& ProcessModifier::getProcess() const {
        return _process;
    }
    bool ProcessModifier::isAttached() const {
        return getProcess() != nullptr;
    }

    void ProcessModifier::onAttach() {
    }
    void ProcessModifier::onInitialize() {
    }
    void ProcessModifier::onUpdate(const float& dt) {
    }
    void ProcessModifier::onKill() {
    }
    void ProcessModifier::onTogglePause() {
    }

    void ProcessModifier::attach(Process* const& process) {
        _process = process;
        onAttach();
    }
}
