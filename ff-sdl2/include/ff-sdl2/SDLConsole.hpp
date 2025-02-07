/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SDL_CONSOLE_HPP
#define _FAITHFUL_FOUNTAIN_SDL_CONSOLE_HPP

#include <ff/Console.hpp>

namespace ff {
class SDLConsole : public ff::Console {
    public:
        virtual ~SDLConsole() = default;

    protected:
        void logImpl(const std::string& msg);
        void warnImpl(const std::string& msg);
        void errorImpl(const std::string& msg);
    };
}

#endif
