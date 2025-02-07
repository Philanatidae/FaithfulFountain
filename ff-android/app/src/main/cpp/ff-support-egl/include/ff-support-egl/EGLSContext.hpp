/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SUPPORT_EGL_EGLS_CONTEXT_HPP
#define _FAITHFUL_FOUNTAIN_SUPPORT_EGL_EGLS_CONTEXT_HPP

#include <ff-support-egl/egl.hpp>

namespace ff {
    class EGLSContext final {
    public:
        EGLSContext(EGLDisplay display, EGLContext context);
        EGLSContext();
        EGLSContext(EGLSContext const& other) = delete;
        ~EGLSContext();

        EGLSContext(EGLSContext&& other);
        EGLSContext& operator=(EGLSContext&& other) noexcept;

        EGLDisplay getEGLDisplay() const;
        EGLContext getEGLContext() const;

        bool good();
        bool bad();

        void destroy(EGLenum* const& errorOut = nullptr);

    private:
        EGLDisplay _display;
        EGLContext _context;
    };
}

#endif
