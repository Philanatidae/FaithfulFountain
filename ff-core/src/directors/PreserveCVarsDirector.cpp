/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/directors/PreserveCVarsDirector.hpp>

#include <ff/CVars.hpp>
#include <ff/Console.hpp>
#include <ff/Locator.hpp>

namespace ff {
    PreserveCVarsDirector::PreserveCVarsDirector() {
    }

    void PreserveCVarsDirector::onInitialize() {
        Locator::getMessageBus().addHandler<PreserveCVarsCmd>(this);
    }

    std::unique_ptr<typename PreserveCVarsCmd::Ret> PreserveCVarsDirector::handleCmd(PreserveCVarsCmd const& cmd) {
        CVars::preserveCVars();

        return std::make_unique<typename PreserveCVarsCmd::Ret>();
    }
}
