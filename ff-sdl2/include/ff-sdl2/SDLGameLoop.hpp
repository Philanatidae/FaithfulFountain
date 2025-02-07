/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_SDL_GAME_LOOP_HPP
#define _FF_SDL_GAME_LOOP_HPP

#include <ff/GameServicer.hpp>

#include <ff-sdl2/SDLWindow.hpp>

namespace ff {

class SDLGameLoop final : public GameLoop {
public:
    SDLGameLoop();

    void init(Game* const& gamePtr) override;
    void update(const float& dt) override;
    void pollEvents() override;
    void onService() override;
    void onShutdown() override;
    float getTargetRefreshRate() override;
    glm::ivec2 getBackBufferDimensions() override;

    SDLWindow* getMainWindow();

private:
    std::unique_ptr<SDLWindow> _mainWindow;

    bool _shuttingDown;
};

}

#endif
