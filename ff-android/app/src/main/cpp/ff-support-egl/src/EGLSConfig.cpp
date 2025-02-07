/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-support-egl/EGLSConfig.hpp>

namespace ff {
    EGLSConfig::EGLSConfig(EGLDisplay display, EGLConfig config)
        :_display(display),_config(config) {
    }
    EGLSConfig::EGLSConfig()
        :_display(EGL_NO_DISPLAY),_config(nullptr) {
    }

    EGLDisplay EGLSConfig::getEGLDisplay() const {
        return _display;
    }
    EGLConfig EGLSConfig::getEGLConfig() const {
        return _config;
    }

    bool EGLSConfig::good() const {
        return !bad();
    }
    bool EGLSConfig::bad() const {
        return _config == nullptr;
    }

    void EGLSConfig::destroy() {
        _display = EGL_NO_DISPLAY;
        _config = nullptr;
    }

    EGLint EGLSConfig::getConfigAttribute(EGLint const& attribute, EGLenum* const& errorOut) const {
        EGLint value;
        if(eglGetConfigAttrib(_display, _config, attribute, &value) == EGL_FALSE) {
            EGLenum error = eglGetError();
            if(errorOut != nullptr) {
                *errorOut = error;
            }
            FF_EGL_PRINT_FRIENDLY_ERROR(error);
            return -1;
        }
        return value;
    }

    EGLint EGLSConfig::getRedSize(EGLenum* const& errorOut) const {
        return getConfigAttribute(EGL_RED_SIZE, errorOut);
    }
    EGLint EGLSConfig::getBlueSize(EGLenum* const& errorOut) const {
        return getConfigAttribute(EGL_GREEN_SIZE, errorOut);
    }
    EGLint EGLSConfig::getGreenSize(EGLenum* const& errorOut) const {
        return getConfigAttribute(EGL_BLUE_SIZE, errorOut);
    }

    EGLint EGLSConfig::getDepthSize(EGLenum* const& errorOut) const {
        return getConfigAttribute(EGL_DEPTH_SIZE, errorOut);
    }
}
