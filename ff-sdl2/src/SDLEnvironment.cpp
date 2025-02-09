/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-sdl2/SDLEnvironment.hpp>

#if !defined(__APPLE__)
#define WIN32_LEAN_AND_MEAN
#include <platform_folders.h>
#endif

#include <ff/CVars.hpp>
#include <ff/Console.hpp>
#include <ff/Locator.hpp>

namespace ff {

SDLEnvironment::SDLEnvironment()
    :_dataStorage(getConfigDirectory()),
    _capabilities(0) {
}
SDLEnvironment::~SDLEnvironment() {
    for(int i = 0; i < _cursors.size(); i++) {
        if(_cursors[i] == nullptr) {
            continue;
        }
        SDL_FreeCursor(_cursors[i]);
    }
}

void SDLEnvironment::init() {
    FF_CONSOLE_LOG("Config directory: `%s`", getConfigDirectory());

    Locator::getMessageBus().addHandler<EnvOpenURLCmd>(this);
    _capabilities |= EnvironmentCapabilities::OPEN_URL;

    Locator::getMessageBus().addHandler<EnvBeginTextInputCmd>(this);
    Locator::getMessageBus().addHandler<EnvEndTextInputCmd>(this);

    _capabilities |= EnvironmentCapabilities::MOUSE_CURSORS;
    Locator::getMessageBus().addHandler<EnvCursorChangeCmd>(this);
    for(int i = 0; i < _cursors.size(); i++) {
        if(i >= (int)MouseCursor::COUNT) {
            _cursors[i] = nullptr;
            continue;
        }
        MouseCursor engineCursor = (MouseCursor)i;
        switch(engineCursor) {
        case MouseCursor::IBEAM:
            _cursors[i] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
            break;
        case MouseCursor::HAND:
            _cursors[i] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
            break;
        case MouseCursor::RESIZE_EW:
            _cursors[i] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
            break;
        case MouseCursor::RESIZE_NS:
            _cursors[i] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
            break;
        case MouseCursor::RESIZE_ALL:
            _cursors[i] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
            break;
        case MouseCursor::RESIZE_NESW:
            _cursors[i] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
            break;
        case MouseCursor::RESIZE_NWSE:
            _cursors[i] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
            break;
        case MouseCursor::NOT_ALLOWED:
            _cursors[i] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
            break;
        // @todo Wait cursor
        case MouseCursor::DEFAULT:
        default:
            _cursors[i] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
            break;
        }
    }

    _capabilities |= EnvironmentCapabilities::CLIPBOARD;
    Locator::getMessageBus().addHandler<EnvCopyToClipboardCmd>(this);

#if defined(WIN32)
    _capabilities |= EnvironmentCapabilities::SYSTEM_GAMEPAD_ASSIGNMENT;
#endif
}

std::string SDLEnvironment::getName() const {
#if defined(__APPLE__)
    return "macOS";
#elif defined(WIN32)
    return "Windows";
#else
    return "Linux";
#endif
}
EnvironmentCapabilities_t SDLEnvironment::getCapabilities() const {
    // @todo Support Windows, Linux motion sensors?
    return _capabilities;
}

IDataStorage const& SDLEnvironment::getDataStorage() const {
    return _dataStorage;
}

void SDLEnvironment::setReportGravity(bool const& enable) {
    FF_CONSOLE_ERROR("Environment does not have gravity motion capability.");
}
void SDLEnvironment::setReportOrientation(bool const& enable) {
    FF_CONSOLE_ERROR("Environment does not have orientation motion capability.");
}
void SDLEnvironment::setReportAcceleration(bool const& enable) {
    FF_CONSOLE_ERROR("Environment does not have acceleration motion capability.");
}

bool SDLEnvironment::isReportingGravity() const {
    return false;
}
bool SDLEnvironment::isReportingOrientation() const {
    return false;
}
bool SDLEnvironment::isReportingAcceleration() const {
    return false;
}

std::string SDLEnvironment::getClipboard() const {
    char* clipboardTextPtr = SDL_GetClipboardText();
    std::string clipboard(clipboardTextPtr);
    SDL_free(clipboardTextPtr);
    return clipboard;
}

bool SDLEnvironment::isRecordingTextInput() const {
    return SDL_IsTextInputActive() == SDL_TRUE;
}

#if !defined(__APPLE__)
// macOS path requires `NSHomeDirectory()`, so it is in DesktopEnvironment.mm
std::string SDLEnvironment::getConfigDirectory() const {
    // Windows & Linux are the same; config path + package name
    // @todo Attempt to remove dependency on Sago, at least for Windows
    // @todo There's a platform equivalent to NSHomeDirectory on Windows.
    // @todo Can Linux be automated here? Would be nice if we could make it abstract for BSD support.
    return (std::filesystem::path(sago::getConfigHome())/tinyformat::format("%s.%s", CVars::get<std::string>("package_prefix"), CVars::get<std::string>("game_name")).c_str()).string();
}
#endif

std::unique_ptr<typename EnvOpenURLCmd::Ret> SDLEnvironment::handleCmd(EnvOpenURLCmd const& cmd) {
    std::string fullAddress = tinyformat::format("https://%s", cmd.address);
    SDL_OpenURL(fullAddress.c_str());

    return std::make_unique<typename EnvOpenURLCmd::Ret>();
}
std::unique_ptr<typename EnvBeginTextInputCmd::Ret> SDLEnvironment::handleCmd(EnvBeginTextInputCmd const& cmd) {
    // @todo SDL_SetTextInputRect
    SDL_StartTextInput();

    return std::make_unique<typename EnvBeginTextInputCmd::Ret>();
}
std::unique_ptr<typename EnvEndTextInputCmd::Ret> SDLEnvironment::handleCmd(EnvEndTextInputCmd const& cmd) {
    SDL_StopTextInput();

    return std::make_unique<typename EnvEndTextInputCmd::Ret>();
}
std::unique_ptr<typename EnvCopyToClipboardCmd::Ret> SDLEnvironment::handleCmd(EnvCopyToClipboardCmd const& cmd) {
    if(SDL_SetClipboardText(cmd.value.c_str()) != 0) {
        FF_CONSOLE_ERROR("Could not copy text to clipboard: %s", SDL_GetError());
    }

    return std::make_unique<typename EnvCopyToClipboardCmd::Ret>();
}
std::unique_ptr<typename EnvCursorChangeCmd::Ret> SDLEnvironment::handleCmd(EnvCursorChangeCmd const& cmd) {
    if(cmd.cursor != MouseCursor::UNKNOWN) {
        SDL_SetCursor(_cursors[(int)cmd.cursor]);
    } else {
        FF_CONSOLE_ERROR("Attempted to change to UNKNOWN mouse cursor.");
    }

    return std::make_unique<typename EnvCursorChangeCmd::Ret>();
}

}

