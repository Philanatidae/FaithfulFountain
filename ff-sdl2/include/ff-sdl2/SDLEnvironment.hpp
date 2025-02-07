/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SDL_ENVIRONMENT_HPP
#define _FAITHFUL_FOUNTAIN_SDL_ENVIRONMENT_HPP

#include <ff/env/IEnvironment.hpp>

#include <ff/env/StdDataStorage.hpp>

#include <ff/messages/CmdHandler.hpp>
#include <ff/commands/env/EnvOpenURLCmd.hpp>

#include <ff/commands/env/EnvBeginTextInputCmd.hpp>
#include <ff/commands/env/EnvEndTextInputCmd.hpp>

#include <ff/commands/env/EnvCopyToClipboardCmd.hpp>

#include <ff/commands/env/EnvCursorChangeCmd.hpp>

#include <array>

#include <SDL.h>

namespace ff {

class SDLEnvironment final : public IEnvironment,
    public CmdHandler<EnvOpenURLCmd>,
    public CmdHandler<EnvBeginTextInputCmd>,
    public CmdHandler<EnvEndTextInputCmd>,
    public CmdHandler<EnvCopyToClipboardCmd>,
    public CmdHandler<EnvCursorChangeCmd> {
public:
    SDLEnvironment();
    ~SDLEnvironment();

    void init() override;

    std::string getName() const override;
    EnvironmentCapabilities_t getCapabilities() const override;

    IDataStorage const& getDataStorage() const override;

    void setReportGravity(bool const& enable) override;
    void setReportOrientation(bool const& enable) override;
    void setReportAcceleration(bool const& enable) override;

    bool isReportingGravity() const override;
    bool isReportingOrientation() const override;
    bool isReportingAcceleration() const override;

    std::string getClipboard() const override;

    bool isRecordingTextInput() const override;

    std::string getConfigDirectory() const;

    std::unique_ptr<typename EnvOpenURLCmd::Ret> handleCmd(EnvOpenURLCmd const& cmd) override;
    std::unique_ptr<typename EnvBeginTextInputCmd::Ret> handleCmd(EnvBeginTextInputCmd const& cmd) override;
    std::unique_ptr<typename EnvEndTextInputCmd::Ret> handleCmd(EnvEndTextInputCmd const& cmd) override;
    std::unique_ptr<typename EnvCopyToClipboardCmd::Ret> handleCmd(EnvCopyToClipboardCmd const& cmd) override;
    std::unique_ptr<typename EnvCursorChangeCmd::Ret> handleCmd(EnvCursorChangeCmd const& cmd) override;

private:
    StdDataStorage _dataStorage;
    EnvironmentCapabilities_t _capabilities;

    std::array<SDL_Cursor*, (size_t)MouseCursor::COUNT> _cursors;
};

}

#endif

