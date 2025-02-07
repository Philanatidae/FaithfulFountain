/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SUPPORT_EGL_EGLS_CONFIG_HPP
#define _FAITHFUL_FOUNTAIN_SUPPORT_EGL_EGLS_CONFIG_HPP

#include <ff-support-egl/egl.hpp>

namespace ff {
    class EGLSConfig final {
    public:
        EGLSConfig(EGLDisplay display, EGLConfig config);
        EGLSConfig();
        ~EGLSConfig() = default;

        EGLDisplay getEGLDisplay() const;
        EGLConfig getEGLConfig() const;

        bool good() const;
        bool bad() const;

        void destroy();

        EGLint getConfigAttribute(EGLint const& attribute, EGLenum* const& errorOut = nullptr) const;

        EGLint getRedSize(EGLenum* const& errorOut = nullptr) const;
        EGLint getBlueSize(EGLenum* const& errorOut = nullptr) const;
        EGLint getGreenSize(EGLenum* const& errorOut = nullptr) const;

        EGLint getDepthSize(EGLenum* const& errorOut = nullptr) const;

    private:
        EGLDisplay _display;
        EGLConfig _config;
    };
}

#endif
