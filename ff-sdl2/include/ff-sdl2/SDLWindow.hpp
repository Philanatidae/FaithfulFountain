/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SDL_WINDOW_HPP
#define _FAITHFUL_FOUNTAIN_SDL_WINDOW_HPP

#include <SDL.h>

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff-sdl2/commands/ResizeWindowCmd.hpp>
#include <ff/commands/input/AbsoluteMouseMovementCmd.hpp>
#include <ff/commands/input/RelativeMouseMovementCmd.hpp>

#include <ff/Game.hpp>

#include <ff-sdl2/SDLGamepadManager.hpp>

namespace ff {

class SDLWindow final
    : public CmdHandler<ResizeWindowCmd>,
    public CmdHandler<AbsoluteMouseMovementCmd>,
    public CmdHandler<RelativeMouseMovementCmd> {
public:
    SDLWindow(Game* const& game,
        int const& width,
        int const& height,
        std::string const& title);
    ~SDLWindow();

    bool isValid() const;

    bool getShouldClose();

    void show();
    void hide();

    void setTitle(std::string const& title);

    glm::ivec2 getFrameBufferSize() const;
    double getRefreshRate() const;

    SDL_Window* getSDLWindowPtr() const;

    void pollEvents();

    std::unique_ptr<typename ResizeWindowCmd::Ret> handleCmd(ResizeWindowCmd const& cmd) override;
    std::unique_ptr<typename AbsoluteMouseMovementCmd::Ret> handleCmd(AbsoluteMouseMovementCmd const& cmd) override;
    std::unique_ptr<typename RelativeMouseMovementCmd::Ret> handleCmd(RelativeMouseMovementCmd const& cmd) override;

private:
    std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> _windowPtr;
    bool _shouldClose;

#if defined(FF_HAS_GRAPHICS_OPENGL)
    SDL_GLContext _glContext = nullptr;
#endif

    glm::vec2 _mousePos;

    SDLGamepadManager _gamepadManager;

    void handleEventKey(SDL_Event& event);
    void handleEventMouseMotion(SDL_Event& event);
    void handleEventMouseButton(SDL_Event& event);
    void handleEventMouseWheel(SDL_Event& event);

    void handleEventTextInput(SDL_Event& event);
    void handleEventTextEditing(SDL_Event& event);

    void handleEventWindow(SDL_Event& event);
    void handleEventWindowResized(SDL_WindowEvent& event);
};

}

#endif

