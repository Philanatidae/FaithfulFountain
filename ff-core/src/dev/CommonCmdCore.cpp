/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/dev/CommonCmdCore.hpp>

#include <ff/Locator.hpp>
#include <ff/messages/CmdRegistrar.hpp>

#include <ff/io/StringSerializer.hpp>

#include <magic_enum.hpp>

namespace ff {

std::unique_ptr<HelpCmd::Ret> CommonCmdCore::handleCmd(HelpCmd const& cmd) {
    if(cmd.cmd == std::nullopt) {
        FF_CONSOLE_LOG("List of commands:");
        auto cmds = CmdRegistrar::getCmds();
        for(auto const& cmdName : cmds) {
            FF_CONSOLE_LOG(" - %s", cmdName);
        }

        return std::make_unique<HelpCmd::Ret>();
    }
    std::string cmdName = cmd.cmd.value();
    if (cmdName.compare(0, 4, "cmd_") != 0) {
        cmdName = "cmd_" + cmdName;
    }
    std::string desc;
    if(CmdRegistrar::getCmdDescription(cmdName, &desc)) { 
        FF_CONSOLE_LOG("%s ~ %s", cmdName, desc);
        MetadataSerializer metadataSerializer;
        FF_ASSERT(CmdRegistrar::analyzeCmd(cmdName, metadataSerializer),
            "Coult not analyze cmd %s", cmdName)
        auto args = metadataSerializer.getArgs();
        for(auto& arg : args) {
            FF_CONSOLE_LOG(" - %s%s (%s)", arg.name, arg.optional ? "?" : "", magic_enum::enum_name(arg.type));
        }
    } else {
        FF_CONSOLE_ERROR("Unknown command: %s", cmd.cmd.value());
    }

    return std::make_unique<HelpCmd::Ret>();
}
std::unique_ptr<EchoCmd::Ret> CommonCmdCore::handleCmd(EchoCmd const& cmd) {
    FF_CONSOLE_LOG("%s", cmd.msg);

    return std::make_unique<EchoCmd::Ret>();
}

std::unique_ptr<PrintCmd::Ret> CommonCmdCore::handleCmd(PrintCmd const& cmd) {
    if(CVars::getRawCVar(cmd.cvar) != nullptr) {
        FF_CONSOLE_LOG("%s", CVars::getRawCVar(cmd.cvar)->toString());
    } else {
        FF_CONSOLE_ERROR("`%s` is not a valid CVar.", cmd.cvar);
    }

    return std::make_unique<PrintCmd::Ret>();
}
std::unique_ptr<SetCmd::Ret> CommonCmdCore::handleCmd(SetCmd const& cmd) {
    if(CVars::getRawCVar(cmd.cvar) != nullptr) {
        StringSerializer serializer;
        serializer.setString(cmd.val);
        serializer.setDirection(SerializerDirection::READ);
        CVars::getRawCVar(cmd.cvar)->serialize(serializer);
        FF_CONSOLE_LOG("%s", CVars::getRawCVar(cmd.cvar)->toString());
    } else {
        FF_CONSOLE_ERROR("`%s` is not a valid CVar.", cmd.cvar);
    }

    return std::make_unique<SetCmd::Ret>();
}

            /*std::vector<std::string> cmdSegments = split(_consoleCmdEntry, ' ');
            if(cmdSegments.size() > 0) {
                FF_CONSOLE_LOG("Command: %s", cmdSegments[0]);
            }*/
            /*
            if(cmdSegments[0] == "clear") {
                Console::getLog().clear();
            } else if(cmdSegments[0] == "echo") {
                if(cmdSegments.size() > 2) {
                    FF_CONSOLE_ERROR("`echo` requires exactly one parameter.");
                } else {
                    if(CVars::getRawCVar(cmdSegments[1]) != nullptr) {
                        FF_CONSOLE_LOG("%s", CVars::getRawCVar(cmdSegments[1])->toString());
                    } else {
                        FF_CONSOLE_ERROR("`%s` is not a valid CVar.", cmdSegments[1]);
                    }
                }
            } else {
                if(cmdSegments[0] == "help"
                    && cmdSegments.size() == 2
                    && CVars::getRawCVar(cmdSegments[1]) != nullptr) {
                    FF_CONSOLE_LOG("%s: %s", cmdSegments[1], CVars::getRawCVar(cmdSegments[1])->getDescription());
                } else {
                    FF_CONSOLE_LOG("`help` - Prints help text for a CVar.");
                }
            }*/

void CommonCmdCore::onInitialize() {
    Locator::getMessageBus().addHandler<HelpCmd>(this);
    Locator::getMessageBus().addHandler<EchoCmd>(this);

    Locator::getMessageBus().addHandler<PrintCmd>(this);
    Locator::getMessageBus().addHandler<SetCmd>(this);
}

}

