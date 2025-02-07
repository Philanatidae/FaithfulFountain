/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-support-egl/EGLSContext.hpp>

namespace ff {
    EGLSContext::EGLSContext(EGLDisplay display, EGLContext context)
        :_display(display),_context(context) {
    }
    EGLSContext::EGLSContext()
        :EGLSContext(EGL_NO_DISPLAY, EGL_NO_CONTEXT) {
    }
    EGLSContext::~EGLSContext() {
        destroy();
    }

    EGLSContext::EGLSContext(EGLSContext&& other)
            :_display(other._display),_context(other._context) {
        other._display = EGL_NO_DISPLAY;
        other._context = EGL_NO_CONTEXT;
    }
    EGLSContext& EGLSContext::operator=(EGLSContext&& other) noexcept {
        _display = other._display;
        _context = other._context;

        other._display = EGL_NO_DISPLAY;
        other._context = EGL_NO_CONTEXT;

        return *this;
    }

    EGLDisplay EGLSContext::getEGLDisplay() const {
        return _display;
    }
    EGLContext EGLSContext::getEGLContext() const {
        return _context;
    }

    bool EGLSContext::good() {
        return !bad();
    }
    bool EGLSContext::bad() {
        return _display == EGL_NO_CONTEXT;
    }

    void EGLSContext::destroy(EGLenum* const& errorOut) {
        if (_context != EGL_NO_CONTEXT) {
            if (eglDestroyContext(_display, _context) == EGL_FALSE) {
                EGLenum error = eglGetError();
                if (errorOut != nullptr) {
                    *errorOut = error;
                }
                FF_EGL_PRINT_FRIENDLY_ERROR(error);
            }
            _context = EGL_NO_CONTEXT;
            _display = EGL_NO_DISPLAY;
        }
    }
}
