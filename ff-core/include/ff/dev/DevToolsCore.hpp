/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_DEV_DEV_TOOLS_CORE_HPP
#define _FAITHFUL_FOUNTAIN_DEV_DEV_TOOLS_CORE_HPP

#include <ff/processes/Process.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff/events/dev/PopulateImGuiFrameEvent.hpp>
#include <ff/events/input/KeyboardKeyDownEvent.hpp>

#include <string>

namespace ff {

class DevToolsCore final : public Process,
    public EventListener<PopulateImGuiFrameEvent>,
    public EventListener<KeyboardKeyDownEvent> {
public:
    DevToolsCore();
    ~DevToolsCore() = default;

protected:
    bool processEvent(PopulateImGuiFrameEvent const& evt) override;
    bool processEvent(KeyboardKeyDownEvent const& evt) override;

    void onInitialize() override;

private:
    std::string _consoleCmdEntry;
    std::string _editingCVar;

    void consoleWindow();
    void cvarsWindow();
    void statisticsWindow();
};

}

#endif
