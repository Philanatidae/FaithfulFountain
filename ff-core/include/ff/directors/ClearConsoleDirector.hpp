/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_DIRECTORS_CLEAR_CONSOLE_DIRECTOR_HPP
#define _FF_DIRECTORS_CLEAR_CONSOLE_DIRECTOR_HPP

#include <ff/processes/Process.hpp>

#include <ff/messages/CmdHandler.hpp>
#include <ff/commands/ClearCmd.hpp>

namespace ff {

class ClearConsoleDirector final : public Process,
    public CmdHandler<ClearCmd> {
public:
    ~ClearConsoleDirector() = default;

    std::unique_ptr<ClearCmd::Ret> handleCmd(ClearCmd const& cmd) override;

protected:
    void onInitialize() override;
};

}

#endif

