/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_HPP

#include <epoxy/gl.h>

namespace ff {
    bool checkOpenGLErrors(char const* const funcName, char const* const fileName, int lineNumber);
}

#if defined(FF_DEV_FEATURES)
#include <ff/Console.hpp>
#include <ff/CVars.hpp>
#include <string>

template<typename... Args>
std::string gl_printFuncArgs(Args... args) {
    std::string str;
    int i = 0;
    ([&] {
        if (i++ > 0) {
            str += ", ";
        }
        str += tinyformat::format("%s", args);
    }(), ...);
    return str;
}

#define FF_GL_CALL(func, ...) \
    [&]() { \
        if(ff::CVars::get<bool>("graphics_gl_print_api_calls")) { \
            FF_CONSOLE_LOG(#func"("#__VA_ARGS__") => "#func"(%s)", gl_printFuncArgs(__VA_ARGS__)); \
        } \
        return func(__VA_ARGS__);\
    }(); \
    if(ff::CVars::get<bool>("graphics_gl_check_call_errors")) { \
        ff::checkOpenGLErrors(#func, (__FILE__), (__LINE__)); \
    }
#else
#define FF_GL_CALL(func, ...) \
    func(__VA_ARGS__);
#endif

#endif
