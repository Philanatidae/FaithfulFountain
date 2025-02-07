/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/RenderCore.hpp>

#include <ff/Locator.hpp>

#include <ff/commands/render/RenderSceneForCameraCmd.hpp>
#include <ff/commands/render/RenderImGuiCmd.hpp>

#include <imgui.h>

namespace ff {

RenderCore::RenderCore(ProcessManager* const& processManager)
    :_processManager(processManager),
    _elapsed(0) {
}

std::unique_ptr<RenderCmd::Ret> RenderCore::handleCmd(RenderCmd const& cmd) {
    Locator::getStatistics().beginStopwatch("game_render_time");

    // Render game scene for debug view
    Locator::getMessageBus().dispatch<RenderSceneForCameraCmd>("debug",
        Locator::getGraphicsDevice().getPresentationRenderTarget(),
        cmd.betweenFrameAlpha,
        true);
    // Render game scene for game view
    Locator::getMessageBus().dispatch<RenderSceneForCameraCmd>("gameplay",
        _sceneGameCameraTarget->getColorRenderTarget(),
        cmd.betweenFrameAlpha,
        false);

    _renderPipeline->render();

    Locator::getMessageBus().dispatch<RenderImGuiCmd>(cmd.betweenFrameAlpha, cmd.dt, "debug");

    Locator::getStatistics().pushListValue("Game Render Time (ms)", Locator::getStatistics().endStopwatch("game_render_time") * 1000);

    return std::make_unique<RenderCmd::Ret>();
}

bool RenderCore::processEvent(PopulateImGuiFrameEvent const& event) {
    static ImVec2 initialSize = ImVec2(128, 72);
    ImGui::SetNextWindowSize(initialSize, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(FLT_MIN, FLT_MIN), ImVec2(FLT_MAX, FLT_MAX),
        [](ImGuiSizeCallbackData* data) {
        ImVec2 const& initialSize = *reinterpret_cast<ImVec2*>(data->UserData);
        data->DesiredSize = ImVec2(data->DesiredSize.x, data->DesiredSize.x * initialSize.y / initialSize.x);
    },
    (void*)&initialSize);
    ImGui::Begin("Game View",
        nullptr,
        ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoScrollWithMouse);

    _sceneGameCameraTargetColorTexture = _sceneGameCameraTarget->getColorRenderTarget();
    ImTextureID gameViewId = reinterpret_cast<ImTextureID>(&_sceneGameCameraTargetColorTexture);
    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    ImGui::Image(gameViewId, availableSize);

    ImGui::End();

    return false;
}

void RenderCore::onInitialize() {
    initRenderDetails();

    Locator::getCameraManager().addPCamera("debug",
        new PerspectSceneCamera(glm::radians(ff::CVars::get<float>("graphics_camera_gameplay_fov")),
            ff::CVars::get<float>("graphics_camera_gameplay_near")));
    PerspectiveCamera* dcam = Locator::getCameraManager().getPCamera("debug");
    dcam->lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0));
    dcam->update();
    _processManager->attachProcess(_debugCamDirector = std::make_shared<DebugCameraDirector>("debug"));

    Locator::getMessageBus().addHandler<RenderCmd>(this);
    Locator::getMessageBus().addListener<PopulateImGuiFrameEvent>(this);
}

void RenderCore::onUpdate(const float& dt) {
}

void RenderCore::initRenderDetails() {
    auto presentationTarget = Locator::getGraphicsDevice().getPresentationRenderTarget();

    _sceneGameCameraTarget = std::make_unique<FullscreenRenderTarget>(presentationTarget->getFormat(),
        TextureUsage::GPU_RENDER_TARGET
        | TextureUsage::GPU_SAMPLE
        | TextureUsage::GPU_STORE
        | TextureUsage::GPU_LOAD
        | TextureUsage::GPU_BLIT,
        "core_scene_game_camera_target");

    _renderPipeline = std::make_unique<RenderPipeline>();
    _renderPipeline->init();
}

}

