/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SUPPORT_EGL_EGLS_DISPLAY_HPP
#define _FAITHFUL_FOUNTAIN_SUPPORT_EGL_EGLS_DISPLAY_HPP

#include <ff-support-egl/egl.hpp>

#include <ff-support-egl/EGLSConfig.hpp>
#include <ff-support-egl/EGLSConfigAttribList.hpp>

#include <ff-support-egl/EGLSContext.hpp>
#include <ff-support-egl/EGLSSurface.hpp>

#include <vector>

namespace ff {
    class EGLSDisplay final {
    public:
        static EGLSDisplay fromDefaultDisplay();

        EGLSDisplay(EGLDisplay display = EGL_NO_DISPLAY);
        EGLSDisplay(EGLSDisplay const& other) = delete;
        ~EGLSDisplay();

        EGLSDisplay(EGLSDisplay&& other);
        EGLSDisplay& operator=(EGLSDisplay&& other) noexcept;

        EGLDisplay getEGLDisplay() const;

        bool good() const;
        bool bad() const;

        void destroy(EGLenum* const& errorOut = nullptr);

        std::vector<EGLSConfig> getConfigs(EGLenum* const& errorOut = nullptr) const;
        std::vector<EGLSConfig> getConfigs(EGLSConfigAttribList const& attribList, EGLenum* const& errorOut = nullptr) const;

        EGLSContext createContext(EGLSConfig const& config, EGLint const& clientVersion, EGLenum* const& errorOut = nullptr);
        EGLSSurface createSurface(EGLSConfig const& config, EGLNativeWindowType window, EGLenum* const& errorOut = nullptr);

        bool bind(EGLSSurface& drawSurface, EGLSSurface& readSurface, EGLSContext& context, EGLenum* const& errorOut = nullptr);
        bool bind(EGLSSurface& surface, EGLSContext& context, EGLenum* const& errorOut = nullptr);
        bool unbind(EGLenum* const& errorOut = nullptr);

        bool swapBuffers(EGLSSurface& surface, EGLSContext& context, EGLenum* const& errorOut = nullptr);
    private:
        EGLDisplay _display;
    };
}

#endif
