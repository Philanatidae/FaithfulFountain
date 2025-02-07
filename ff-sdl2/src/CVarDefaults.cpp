/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/CVars.hpp>

FF_CVAR_DEFINE(window_width, int, 1280, ff::CVarFlags::READ_ONLY, "Width of the desktop window.")
FF_CVAR_DEFINE(window_height, int, 800, ff::CVarFlags::READ_ONLY, "Height of the desktop window.")
FF_CVAR_DEFINE(window_resizable, bool, true, ff::CVarFlags::READ_ONLY, "Enables whether the window should be resizable.")
FF_CVAR_DEFINE(window_force_initial_aspect_ratio, bool, false, ff::CVarFlags::READ_ONLY, "Forces the window to maintain the aspect ratio defined by `window_width`/`window_height`.")
FF_CVAR_DEFINE(window_macos_disable_fullscreen, bool, false, ff::CVarFlags::READ_ONLY, "Disables green \"fullscreen\" button on macOS.")

FF_CVAR_DEFINE(input_gamepad_trigger_threshold, float, 0.3f, ff::CVarFlags::PRESERVE, "Threshold point for converting analog trigger to button up/down.");

