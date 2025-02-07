/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/directors/ClearConsoleDirector.hpp>

#include <ff/Console.hpp>
#include <ff/Locator.hpp>

namespace ff {

std::unique_ptr<ClearCmd::Ret> ClearConsoleDirector::handleCmd(ClearCmd const& cmd) {
    Console::getLog().clear();

    return std::make_unique<ClearCmd::Ret>();
}

void ClearConsoleDirector::onInitialize() {
    Locator::getMessageBus().addHandler<ClearCmd>(this);
}

}

