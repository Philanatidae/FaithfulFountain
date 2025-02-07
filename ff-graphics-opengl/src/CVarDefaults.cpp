/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/CVars.hpp>

FF_CVAR_DEFINE(graphics_gl_print_api_calls, bool, false, ff::CVarFlags::DEV_PRESERVE, "Prints OpenGL API calls to the console.");
#if defined(FF_DEV_FEATURES)
FF_CVAR_DEFINE(graphics_gl_check_call_errors, bool, true, ff::CVarFlags::PRESERVE, "Calls `glGetError` after every OpenGL function call.")
#else
FF_CVAR_DEFINE(graphics_gl_check_call_errors, bool, false, ff::CVarFlags::PRESERVE, "Calls `glGetError` after every OpenGL function call.")
#endif

FF_CVAR_DEFINE(graphics_gl_use_arb_draw_buffer_blend, bool, true, ff::CVarFlags::PRESERVE, "Use GL_ARB_draw_buffers_blend if available.")
FF_CVAR_DEFINE(graphics_gl_use_arb_texture_storage, bool, true, ff::CVarFlags::PRESERVE, "Use GL_ARB_texture_storage if available.")
FF_CVAR_DEFINE(graphics_gl_use_arb_invalidate_subdata, bool, true, ff::CVarFlags::PRESERVE, "Use GL_ARB_invalidate_subdata if available.")
FF_CVAR_DEFINE(graphics_gl_use_arb_debug_label, bool, true, ff::CVarFlags::PRESERVE, "Use GL_ARB_debug_label if available.")
FF_CVAR_DEFINE(graphics_gl_use_khr_debug, bool, true, ff::CVarFlags::PRESERVE, "Use GL_KHR_debug if available.")
