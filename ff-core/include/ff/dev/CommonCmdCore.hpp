/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_DEV_COMMON_CMD_CORE_HPP
#define _FF_DEV_COMMON_CMD_CORE_HPP

#include <ff/processes/Process.hpp>
#include <ff/messages/CmdHandler.hpp>

#include <ff/commands/HelpCmd.hpp>
#include <ff/commands/EchoCmd.hpp>

#include <ff/commands/PrintCmd.hpp>
#include <ff/commands/SetCmd.hpp>

namespace ff {

class CommonCmdCore final : public Process,
    public CmdHandler<HelpCmd>,
    public CmdHandler<EchoCmd>,
    public CmdHandler<SetCmd>,
    public CmdHandler<PrintCmd> {
public:
    ~CommonCmdCore() = default;

protected:
    std::unique_ptr<HelpCmd::Ret> handleCmd(HelpCmd const& cmd) override;
    std::unique_ptr<EchoCmd::Ret> handleCmd(EchoCmd const& cmd) override;

    std::unique_ptr<PrintCmd::Ret> handleCmd(PrintCmd const& cmd) override;
    std::unique_ptr<SetCmd::Ret> handleCmd(SetCmd const& cmd) override;

    void onInitialize() override;
};

}

#endif

