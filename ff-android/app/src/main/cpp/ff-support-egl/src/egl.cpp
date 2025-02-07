/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-support-egl/egl.hpp>

#include <ff/Console.hpp>

namespace ff {
    void printEGLErrorFriendly(EGLenum const& error, char const *const fileName, int lineNumber) {
        char const* errorStr = "";
        switch(error) {
            case EGL_SUCCESS:
                errorStr = "EGL_SUCCESS";
                break;
            case EGL_NOT_INITIALIZED:
                errorStr = "EGL_NOT_INITIALIZED";
                break;
            case EGL_BAD_ACCESS:
                errorStr = "EGL_BAD_ACCESS";
                break;
            case EGL_BAD_ALLOC:
                errorStr = "EGL_BAD_ALLOC";
                break;
            case EGL_BAD_ATTRIBUTE:
                errorStr = "EGL_BAD_ATTRIBUTE";
                break;
            case EGL_BAD_CONFIG:
                errorStr = "EGL_BAD_CONFIG";
                break;
            case EGL_BAD_CONTEXT:
                errorStr = "EGL_BAD_CONTEXT";
                break;
            case EGL_BAD_CURRENT_SURFACE:
                errorStr = "EGL_BAD_CURRENT_SURFACE";
                break;
            case EGL_BAD_DISPLAY:
                errorStr = "EGL_BAD_DISPLAY";
                break;
            case EGL_BAD_MATCH:
                errorStr = "EGL_BAD_MATCH";
                break;
            case EGL_BAD_NATIVE_PIXMAP:
                errorStr = "EGL_BAD_NATIVE_PIXMAP";
                break;
            case EGL_BAD_NATIVE_WINDOW:
                errorStr = "EGL_BAD_NATIVE_WINDOW";
                break;
            case EGL_BAD_PARAMETER:
                errorStr = "EGL_BAD_PARAMETER";
                break;
            case EGL_CONTEXT_LOST:
                errorStr = "EGL_CONTEXT_LOST";
                break;
        default:
            errorStr = "UNKNOWN";
            break;
        }
        FF_CONSOLE_ERROR("Non-fatal EGL near %s:%s: Error code 0x%04x (%s)", fileName, lineNumber, error, errorStr);
    }
}
