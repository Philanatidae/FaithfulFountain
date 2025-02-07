/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/dev/DevToolsCore.hpp>

#include <ff/Locator.hpp>
#include <ff/CVars.hpp>
#include <ff/Console.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <tinyformat/tinyformat.h>

#include <ff/commands/PreserveCVarsCmd.hpp>

#include <ff/util/String.hpp>

#include <ff/messages/CmdRegistrar.hpp>
#include <ff/io/Serializer.hpp>
#include <ff/io/StringSerializer.hpp>

#include <ff/commands/HelpCmd.hpp>
#include <ff/commands/ClearCmd.hpp>

namespace ff {

DevToolsCore::DevToolsCore() {
}

void DevToolsCore::onInitialize() {
    Locator::getMessageBus().addListener<PopulateImGuiFrameEvent>(this);
    Locator::getMessageBus().addListener<KeyboardKeyDownEvent>(this, MessageListenerPriority::EXTREMELY_HIGH);
}

bool DevToolsCore::processEvent(PopulateImGuiFrameEvent const& evt) {
    consoleWindow();
    statisticsWindow();
    cvarsWindow();

    return false;
}
bool DevToolsCore::processEvent(KeyboardKeyDownEvent const& evt) {
    switch(evt.key) {
    case KeyboardKey::BACKTICK:
        CVars::get<bool>("debug_show_console") = !CVars::get<bool>("debug_show_console");
        return true;
    case KeyboardKey::F1:
        CVars::get<bool>("debug_show_cvars") = !CVars::get<bool>("debug_show_cvars");
        return true;
    case KeyboardKey::F3:
        CVars::get<bool>("debug_show_statistics") = !CVars::get<bool>("debug_show_statistics");
        return true;
    default:
        return false;
    }

    return false;
}

void DevToolsCore::consoleWindow() {
    static bool lastFrameWindowFocused = false;
    if(!CVars::get<bool>("debug_show_console")) {
        lastFrameWindowFocused = false;
        return;
    }

    if(CVars::get<bool>("debug_show_console")) {
        ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
        ImGui::Begin("Developer Console",
            &CVars::get<bool>("debug_show_console"));

        bool const justFocused = ImGui::IsWindowFocused() && !lastFrameWindowFocused;
        lastFrameWindowFocused = ImGui::IsWindowFocused();

        float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y
            + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ConsoleScrollingRegion",
            ImVec2(0, -footer_height_to_reserve),
            false,
            ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
        for(int i = 0; i < Console::getLog().size(); i++) {
            ff::Color color;
            switch(Console::getLog()[i].level) {
            case ConsoleLevel::LOG:
                color = CVars::get<Color>("debug_console_log_color");
                break;
            case ConsoleLevel::WARN:
                color = CVars::get<Color>("debug_console_warn_color");
                break;
            case ConsoleLevel::ERROR:
                color = CVars::get<Color>("debug_console_error_color");
                break;
            default:
                break;
            }
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.r, color.g, color.b, color.a));
            ImGui::TextWrapped("%s", Console::getLog()[i].message.c_str());
            ImGui::PopStyleColor();
        }
        if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();

        static bool justEnteredCommand = false;

        if(justFocused || justEnteredCommand) {
            ImGui::SetKeyboardFocusHere();
            
            justEnteredCommand = false;
        }

        if(ImGui::InputText("##ConsoleInput",
            &_consoleCmdEntry,
            ImGuiInputTextFlags_EnterReturnsTrue)) {
            if(_consoleCmdEntry.size() > 0) {
                StringSerializer serializer;
                serializer.setString(_consoleCmdEntry);
                int argsProvided = serializer.getElementCount() - 1;
                serializer.setDirection(SerializerDirection::READ);
                std::string cmdName;
                serializer.serialize("cmd", cmdName);

                if (cmdName.compare(0, 4, "cmd_") != 0) {
                    cmdName = "cmd_" + cmdName;
                }

                if(argsProvided > 0) {
                    serializer.setString(serializer.getRemainderFromHead());
                }

                MetadataSerializer metadataSerializer;
                if(!CmdRegistrar::analyzeCmd(cmdName, metadataSerializer)) {
                    FF_CONSOLE_ERROR("Unrecognized command: %s", cmdName);
                } else {
                    auto args = metadataSerializer.getArgs();
                    int requiredArgs = 0;
                    for(int i = 0; i < args.size(); i++) {
                        if(args[i].optional) {
                            continue;
                        }
                        requiredArgs++;
                    }

                    if(argsProvided < requiredArgs) {
                        FF_CONSOLE_ERROR("Too few arguments: Expected %s minimum, given %s.", requiredArgs, argsProvided);
                        Locator::getMessageBus().dispatch<HelpCmd>(cmdName);
                    } else if(argsProvided > args.size()) {
                        FF_CONSOLE_ERROR("Too many arguments: Expected %s minimum, given %s.", requiredArgs, argsProvided);
                        Locator::getMessageBus().dispatch<HelpCmd>(cmdName);
                    } else {
                        // Command has been verified to exist, don't need a soft error
                        FF_ASSERT(CmdRegistrar::dispatchCmd(cmdName, serializer),
                            "Command is not registered for dispatch.");
                    }
                }
            }
            _consoleCmdEntry.clear();
            justEnteredCommand = true;
        }
        ImVec2 inputTextPos = ImGui::GetItemRectMin();
        ImVec2 inputTextSize = ImGui::GetItemRectSize();

        std::vector<std::string> relevantCmds = CmdRegistrar::getCmdsForTerm(_consoleCmdEntry);
        if(relevantCmds.size() > 0) {
            // Pop-up with command suggestions
            ImGui::SetNextWindowPos(ImVec2(inputTextPos.x, inputTextPos.y + inputTextSize.y));
            ImGui::SetNextWindowSize(ImVec2(inputTextSize.x, ImGui::GetTextLineHeightWithSpacing() * glm::min<int>(relevantCmds.size(), 5)));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
            if(ImGui::BeginPopup("##CommandSuggestions",
                ImGuiWindowFlags_NoTitleBar
                    | ImGuiWindowFlags_NoResize
                    | ImGuiWindowFlags_NoMove
                    | ImGuiWindowFlags_NoFocusOnAppearing)) {
                for(auto const& cmdName : relevantCmds) {
                    if(ImGui::Selectable(cmdName.c_str())) {
                        _consoleCmdEntry = cmdName;
                        ImGui::CloseCurrentPopup();
                        ImGui::SetKeyboardFocusHere();
                    }
                }
                if(_consoleCmdEntry.size() == 0) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            } else {
                if(_consoleCmdEntry.size() > 0) {
                    ImGui::OpenPopup("##CommandSuggestions");
                }
            }
            ImGui::PopStyleVar();
        }

        ImGui::SameLine();
        if(ImGui::Button("Clear##consoleClear")) {
            Locator::getMessageBus().dispatch<ClearCmd>();
        }

        // Kinda a hack but whatever
        // If the first character is a `, close the console
        if(_consoleCmdEntry.size() > 0 && _consoleCmdEntry[0] == '`') {
            CVars::get<bool>("debug_show_console") = false;
            _consoleCmdEntry = "";
        }

        ImGui::End();
    }
}
void DevToolsCore::cvarsWindow() {
    if(CVars::get<bool>("debug_show_cvars")) {
        ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("CVars Viewer",
            &CVars::get<bool>("debug_show_cvars")); 

        if(ImGui::Button("Save##Control")) {
            Locator::getMessageBus().dispatch<PreserveCVarsCmd>();
        }
        ImGui::SameLine();
        if(ImGui::Button("Load##Control")) {
            FF_CONSOLE_LOG("Loading CVars...");
            CVars::loadCVarsINIFromConfig(CVars::get<std::string>("cvars_config_name"));
        }

        std::vector<std::string> names = CVars::getCVarNames();
        std::sort(names.begin(), names.end());

        ImGui::Columns(4);
        ImGui::Text("CVar");
        ImGui::NextColumn();
        ImGui::Text("Value");
        ImGui::NextColumn();
        ImGui::Text("Default");
        ImGui::NextColumn();
        ImGui::Text("Description");
        ImGui::NextColumn();

        ImGui::Separator();

        for(auto const& name : names) {
            ImGui::TextUnformatted(name.c_str());
            ImGui::NextColumn();

            if((CVars::getRawCVar(name)->getFlags() & CVarFlags::READ_ONLY) != CVarFlags::NONE) {
                ImGui::TextUnformatted(CVars::getRawCVar(name)->toString().c_str());
            } else {
                if(_editingCVar == name) {
                    CVars::getRawCVar(name)->drawImGuiEdit();
                } else {
                    if(ImGui::Button(tinyformat::format("%s##edit%s",
                        CVars::getRawCVar(name)->toString(),
                        name).c_str())) {
                        _editingCVar = name;
                    }
                }
            }
            ImGui::NextColumn();

            if(ImGui::Button(tinyformat::format("%s##reset%s",
                CVars::getRawCVar(name.c_str())->defaultToString(),
                name).c_str())) {
                CVars::getRawCVar(name.c_str())->resetToDefault();
            }
            ImGui::NextColumn();
            
            ImGui::TextWrapped("%s", CVars::getRawCVar(name)->getDescription().c_str());
            ImGui::NextColumn();
        }

        ImGui::End();
    }
}
void DevToolsCore::statisticsWindow() {
    if(CVars::get<bool>("debug_show_statistics")) {
        ImGui::SetNextWindowSize(ImVec2(400, 200),
            ImGuiCond_FirstUseEver);
        ImGui::Begin("Statistics",
            &CVars::get<bool>("debug_show_statistics"));
        if(Locator::getStatistics().getListCount() == 0) {
            ImGui::Text("No statistics.");
        } else {
            Locator::getStatistics().listForEach([](std::string name) -> void {
                ImGui::Text("%s: %f", name.c_str(), Locator::getStatistics().getListTopValue(name));
                ImGui::Text("%s (avg): %f", name.c_str(), Locator::getStatistics().getListAverage(name));
                ImGui::Text("%s (med): %f", name.c_str(), Locator::getStatistics().getListMedian(name));
            });
        }
        ImGui::End();
    }
}

}
