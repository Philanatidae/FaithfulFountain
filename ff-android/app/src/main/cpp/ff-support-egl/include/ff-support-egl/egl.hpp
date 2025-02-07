/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SUPPORT_EGL_EGL_HPP
#define _FAITHFUL_FOUNTAIN_SUPPORT_EGL_EGL_HPP

#include <EGL/egl.h>

namespace ff {
    void printEGLErrorFriendly(EGLenum const& error, char const *const fileName, int lineNumber);
}

#define FF_EGL_PRINT_FRIENDLY_ERROR(error) \
    ff::printEGLErrorFriendly((error), (__FILE__), (__LINE__));

#endif
