/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-sdl2/SDLGameLoop.hpp>

#include <ff/CVars.hpp>
#include <ff/Locator.hpp>

namespace ff {

SDLGameLoop::SDLGameLoop()
    :_shuttingDown(false) {
}

void SDLGameLoop::init(Game* const& gamePtr) {
    _mainWindow = std::make_unique<SDLWindow>(gamePtr,
        CVars::get<int>("window_width"),
        CVars::get<int>("window_height"),
        CVars::get<std::string>("game_name"));
    FF_ASSERT(_mainWindow->isValid(), "Could not create window.");
}
void SDLGameLoop::update(const float& dt) {
}
void SDLGameLoop::pollEvents() {
    _mainWindow->pollEvents();
}
void SDLGameLoop::onService() {
    if(_mainWindow->getShouldClose()
        && !_shuttingDown) {
        ff::Locator::getMessageBus().dispatch<ShutdownCmd>();
        _shuttingDown = true;
    }

#if defined(FF_HAS_GRAPHICS_OPENGL)
    if(Locator::getGraphicsDevice().getBackendName() == GraphicsBackend::OPENGL) {
        // @todo Support adaptive vsync
        SDL_GL_SetSwapInterval(CVars::get<bool>("graphics_vsync") ? 1 : 0);
        SDL_GL_SwapWindow(_mainWindow->getSDLWindowPtr());
    }
#endif
}
void SDLGameLoop::onShutdown() {
    _mainWindow->hide();
    _shuttingDown = true;
}
float SDLGameLoop::getTargetRefreshRate() {
    return _mainWindow->getRefreshRate();
}
glm::ivec2 SDLGameLoop::getBackBufferDimensions() {
    return _mainWindow->getFrameBufferSize();
}

SDLWindow* SDLGameLoop::getMainWindow() {
    return _mainWindow.get();
}

}
