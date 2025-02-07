/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-sdl2/SDLConsole.hpp>

#include <SDL.h>

namespace ff {

void SDLConsole::logImpl(const std::string& msg) {
#if defined(FF_STD_CONSOLE_COLOR)
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
        SDL_LOG_PRIORITY_INFO,
        "%s%s%s",
        StdConsoleColor::LIGHT_CYAN,
        msg.c_str(),
        StdConsoleColor::NO_COLOR);
#else
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
        SDL_LOG_PRIORITY_INFO,
        msg.c_str());
#endif
}
void SDLConsole::warnImpl(const std::string& msg) {
#if defined(FF_STD_CONSOLE_COLOR)
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
        SDL_LOG_PRIORITY_WARN,
        "%s%s%s",
        StdConsoleColor::YELLOW,
        msg.c_str(),
        StdConsoleColor::NO_COLOR);
#else
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
        SDL_LOG_PRIORITY_WARN,
        msg.c_str());
#endif
}
void SDLConsole::errorImpl(const std::string& msg) {
#if defined(FF_STD_CONSOLE_COLOR)
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
        SDL_LOG_PRIORITY_ERROR,
        "%s%s%s",
        StdConsoleColor::LIGHT_RED,
        msg.c_str(),
        StdConsoleColor::NO_COLOR);
#else
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
        SDL_LOG_PRIORITY_ERROR,
        msg.c_str());
#endif
}

}
