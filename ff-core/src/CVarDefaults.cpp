/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/CVars.hpp>
#include <ff/util/OS.hpp>
#include <ff/graphics/Color.hpp>

FF_CVAR_DEFINE(verse, std::string, "Trust in the LORD with all your heart and lean not on your own understanding; in all your ways submit to him, and he will make your paths straight. Proverbs 3:5-6.", ff::CVarFlags::READ_ONLY, "Inspirational quote for this project.")

FF_CVAR_DEFINE(game_name, std::string, "FFRollGame", ff::CVarFlags::READ_ONLY, "Internal name of the game.")
FF_CVAR_DEFINE(package_name, std::string, "com.philanatidae.RollGame", ff::CVarFlags::READ_ONLY, "Package name of the game.");

FF_CVAR_DEFINE(cvars_config_name, std::string, "cvars.ini", ff::CVarFlags::READ_ONLY, "Default name of CVars INI config.")
FF_CVAR_DEFINE(cvar_defaults_config_name, std::string, "cvar_defaults.ini", ff::CVarFlags::READ_ONLY, "Default name of CVar Defaults INI config.")

FF_CVAR_DEFINE(debug_break_on_assert, bool, false, ff::CVarFlags::NONE, "Attempt to break debugger on FF_ASSERT failure.")
FF_CVAR_DEFINE(debug_statistics_list_max_size, int, 20, ff::CVarFlags::DEV_PRESERVE, "Default max size of Statistics lists.")
FF_CVAR_DEFINE(debug_console_log_limit, int, 2048, ff::CVarFlags::DEV_PRESERVE, "Maximum number of console entries stored.");
FF_CVAR_DEFINE(debug_console_log_color, ff::Color, ff::Color(0x00FFF9FF), ff::CVarFlags::DEV_PRESERVE, "Color of console log messages.");
FF_CVAR_DEFINE(debug_console_warn_color, ff::Color, ff::Color(0xFFBD00FF), ff::CVarFlags::DEV_PRESERVE, "Color of console warning messages.");
FF_CVAR_DEFINE(debug_console_error_color, ff::Color, ff::Color(0xFF0000FF), ff::CVarFlags::DEV_PRESERVE, "Color of console error messages.");
FF_CVAR_DEFINE(debug_show_console, bool, false, ff::CVarFlags::NONE, "Displa/hide the console.")
FF_CVAR_DEFINE(debug_show_cvars, bool, false, ff::CVarFlags::NONE, "Display/hide statistics window.")
FF_CVAR_DEFINE(debug_show_playback, bool, false, ff::CVarFlags::NONE, "Display/hide statistics window.")
FF_CVAR_DEFINE(debug_show_statistics, bool, false, ff::CVarFlags::NONE, "Display/hide statistics window.")
FF_CVAR_DEFINE(debug_show_render_pipeline, bool, true, ff::CVarFlags::NONE, "Display/hide render pipeline window.")

FF_CVAR_DEFINE(debug_cam_speed, float, 10.0f, ff::CVarFlags::DEV_PRESERVE, "Speed of debug camera movement.")
FF_CVAR_DEFINE(debug_cam_sensitivity, float, 100.0f, ff::CVarFlags::DEV_PRESERVE, "Sensitivity of debug camera movement.")

FF_CVAR_DEFINE(game_library_path, std::string, "libRollGamed.dylib", ff::CVarFlags::DEV_PRESERVE, "Path of game dynamic library.")

FF_CVAR_DEFINE(asset_bundle_path, std::string, "./Assets", ff::CVarFlags::PRESERVE, "Path to the asset bundle (directory built using Asset Processor).")

FF_CVAR_DEFINE(tick_frequency, float, 60, ff::CVarFlags::DEV_PRESERVE, "Frequency at which to tick game logic internally.")
FF_CVAR_DEFINE(acculmulator_max_before_reset, float, 2, ff::CVarFlags::DEV_PRESERVE, "Maximum value the acculmulator can contain before resetting to 0.")

FF_CVAR_DEFINE(message_filter, std::string, "evt_keyboard[A-z_]+|evt_mouse[A-z_]+|evt_gamepad[A-z_]+|evt_motion[A-z_]+|evt_imgui[A-z_]+|evt_env_text[A-z_]+|cmd_env_request_cursor[A-z_]+|cmd_render[A-z_]+", ff::CVarFlags::PRESERVE, "Regex filter for events and commands on the developer console (exclusive).")

FF_CVAR_DEFINE(view_viewport_height, float, 720, ff::CVarFlags::READ_ONLY, "Height of the viewport.")
FF_CVAR_DEFINE(view_viewport_fill_safe_area, bool, true, ff::CVarFlags::READ_ONLY, "Fills the viewport to the safe area if enabled. If disabled, `view_viewport_aspect_ratio` is used to constrain the viewport.");
FF_CVAR_DEFINE(view_viewport_aspect_ratio, float, 2/3.0f, ff::CVarFlags::READ_ONLY, "Aspect ratio of the viewport.")
FF_CVAR_DEFINE(view_safe_area_padding_left, float, 0, ff::CVarFlags::HOT_MODIFY, "Left padding for safe area of render area, as multiplier of render area.")
FF_CVAR_DEFINE(view_safe_area_padding_right, float, 0, ff::CVarFlags::HOT_MODIFY, "Right padding for safe area of render area, as multiplier of render area.")
FF_CVAR_DEFINE(view_safe_area_padding_top, float, 0, ff::CVarFlags::HOT_MODIFY, "Top padding for safe area of render area, as multiplier of render area.")
FF_CVAR_DEFINE(view_safe_area_padding_bottom, float, 0, ff::CVarFlags::HOT_MODIFY, "Bottom padding for safe area of render area, as multiplier of render area.")

FF_CVAR_DEFINE(graphics_frame_smoothing, bool, true, ff::CVarFlags::PRESERVE, "Enables transform frame smoothing (aka frame interpolation, not to be confused with delta smoothing).")
// @todo Decide if `graphics` is the correct namespace for delta smoothing.
FF_CVAR_DEFINE(graphics_delta_smoothing, bool, true, ff::CVarFlags::PRESERVE, "Enables delta (dt) smoothing. Only applicable when `graphics_vsync` is enabled.")
FF_CVAR_DEFINE(graphics_vsync, bool, true, ff::CVarFlags::PRESERVE, "Enables vertical synchronization.")
FF_CVAR_DEFINE(graphics_backend, std::string, "default", ff::CVarFlags::PRESERVE, "Selects the graphics backend to use (metal, opengl).")
FF_CVAR_DEFINE(graphics_culling_force_none, bool, false, ff::CVarFlags::DEV_PRESERVE, "Forces no back-face culling.")

FF_CVAR_DEFINE(audio_master_volume, float, 0.2f, ff::CVarFlags::DEV_PRESERVE, "Master volume of the audio core.")
FF_CVAR_DEFINE(audio_backend, std::string, "default", ff::CVarFlags::PRESERVE, "Selects the audio backend to use (coreaudio, portaudio, oboe).")

FF_CVAR_DEFINE(debug_show_update_times, bool, false, ff::CVarFlags::DEV_PRESERVE, "Show update times on screen.");
FF_CVAR_DEFINE(debug_show_render_times, bool, false, ff::CVarFlags::DEV_PRESERVE, "Show render times on screen.");
