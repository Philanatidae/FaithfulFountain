/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_COMMANDS_RENDER_SCENE_FOR_CAMERA_CMD_HPP
#define _FF_COMMANDS_RENDER_SCENE_FOR_CAMERA_CMD_HPP

#include <ff/messages/CmdHelpers.hpp>
#include <optional>

#include <ff/graphics/Texture.hpp>
#include <ff/resources/ResourceHandle.hpp>

namespace ff {

FF_CMD_DEFINE_4_R0(RenderSceneForCameraCmd,
    "cmd_render_scene_for_camera",
    "Render scene to a provided Camera",
    std::string, camera,
    ResourceHandle<ColorTexture>, presentationTarget,
    float, betweenFrameAlpha,
    bool, isInteractiveLayer,
    "Camera: %s, Target: %s, Alpha: %f, Interactive: %s",
    (camera, (std::intptr_t)presentationTarget.get(), betweenFrameAlpha, isInteractiveLayer));

}

#endif


