/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-support-egl/EGLSSurface.hpp>

namespace ff {
    EGLSSurface::EGLSSurface(EGLDisplay display, EGLSurface surface)
        :_display(display),_surface(surface) {
    }
    EGLSSurface::EGLSSurface()
        :EGLSSurface(EGL_NO_DISPLAY, EGL_NO_SURFACE) {
    }
    EGLSSurface::~EGLSSurface() {
        destroy();
    }

    EGLSSurface::EGLSSurface(EGLSSurface&& other)
            :_display(other._display),_surface(other._surface) {
        other._display = EGL_NO_DISPLAY;
        other._surface = EGL_NO_SURFACE;
    }
    EGLSSurface& EGLSSurface::operator=(EGLSSurface&& other) noexcept {
        _display = other._display;
        _surface = other._surface;

        other._display = EGL_NO_DISPLAY;
        other._surface = EGL_NO_SURFACE;

        return *this;
    }


    EGLDisplay EGLSSurface::getEGLDisplay() const {
        return _display;
    }
    EGLSurface EGLSSurface::getEGLSurface() const {
        return _surface;
    }

    bool EGLSSurface::good() const {
        return !bad();
    }
    bool EGLSSurface::bad() const {
        return _surface == EGL_NO_SURFACE;
    }

    void EGLSSurface::destroy(EGLenum* const& errorOut) {
        if(_surface != EGL_NO_SURFACE) {
            if(eglDestroySurface(_display, _surface) == EGL_FALSE) {
                EGLenum error = eglGetError();
                if(errorOut != nullptr) {
                    *errorOut = error;
                }
                FF_EGL_PRINT_FRIENDLY_ERROR(error);
            }
            _surface = EGL_NO_SURFACE;
            _display = EGL_NO_DISPLAY;
        }
    }

    EGLint EGLSSurface::querySurface(EGLint const& attribute, EGLenum* const& errorOut) const {
        if(_surface == EGL_NO_SURFACE) {
            if(errorOut != nullptr) {
                *errorOut = EGL_BAD_SURFACE;
            }
            return -1;
        }

        EGLint value;
        if(eglQuerySurface(_display, _surface, attribute, &value) == EGL_FALSE) {
            EGLenum error = eglGetError();
            if(errorOut != nullptr) {
                *errorOut = error;
            }
            FF_EGL_PRINT_FRIENDLY_ERROR(error);
            return -1;
        }
        return value;
    }

    EGLint EGLSSurface::queryConfigID(EGLenum* const& errorOut) const {
        return querySurface(EGL_CONFIG_ID, errorOut);
    }

    EGLint EGLSSurface::queryWidth(EGLenum* const& errorOut) const {
        return querySurface(EGL_WIDTH, errorOut);
    }
    EGLint EGLSSurface::queryHeight(EGLenum* const& errorOut) const {
        return querySurface(EGL_HEIGHT, errorOut);
    }

    void EGLSSurface::surfaceWasInvalidated() {
        _surface = EGL_NO_DISPLAY;
    }
    void EGLSSurface::displayWasInvalidated() {
        _surface = EGL_NO_SURFACE;
        _display = EGL_NO_DISPLAY;
    }
}
