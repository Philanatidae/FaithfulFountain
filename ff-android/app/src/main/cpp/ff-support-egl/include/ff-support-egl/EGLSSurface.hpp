/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SUPPORT_EGL_EGLS_SURFACE_HPP
#define _FAITHFUL_FOUNTAIN_SUPPORT_EGL_EGLS_SURFACE_HPP

#include <ff-support-egl/egl.hpp>

namespace ff {
    class EGLSSurface final {
    public:
        EGLSSurface(EGLDisplay display, EGLSurface surface);
        EGLSSurface();
        EGLSSurface(EGLSSurface const &other) = delete;
        ~EGLSSurface();

        EGLSSurface(EGLSSurface&& other);
        EGLSSurface& operator=(EGLSSurface&& other) noexcept;

        EGLDisplay getEGLDisplay() const;
        EGLSurface getEGLSurface() const;

        bool good() const;
        bool bad() const;

        void destroy(EGLenum* const& errorOut = nullptr);

        EGLint querySurface(EGLint const& attribute, EGLenum* const& errorOut = nullptr) const;

        EGLint queryConfigID(EGLenum* const& errorOut = nullptr) const;

        EGLint queryWidth(EGLenum* const& errorOut = nullptr) const;
        EGLint queryHeight(EGLenum* const& errorOut = nullptr) const;

    private:
        EGLDisplay _display;
        EGLSurface _surface;

        void surfaceWasInvalidated();
        void displayWasInvalidated();
    };
}

#endif
