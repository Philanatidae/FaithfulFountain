/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/CVars.hpp>

FF_CVAR_DEFINE(asset_builder_input_directory, std::string, "", ff::CVarFlags::PRESERVE, "Input directory for asset builder.")
FF_CVAR_DEFINE(asset_builder_output_directory, std::string, "", ff::CVarFlags::PRESERVE, "Output directory for asset builder.")
FF_CVAR_DEFINE(asset_builder_platform_name, std::string, "", ff::CVarFlags::PRESERVE, "Platform name to build assets for asset builder.")
FF_CVAR_DEFINE(asset_builder_graphics_backend, std::string, "", ff::CVarFlags::PRESERVE, "Graphics backend(s) to build assets for asset builder.")
FF_CVAR_DEFINE(asset_builder_production_build, bool, true, ff::CVarFlags::PRESERVE, "Enable building assets for production.")

FF_CVAR_DEFINE(asset_builder_atlas_maximum_extent, float, 4096.0f, ff::CVarFlags::READ_ONLY, "Maximum extent that a texture atlas can be.")
