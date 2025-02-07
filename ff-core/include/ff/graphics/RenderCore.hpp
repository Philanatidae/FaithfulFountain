/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_GRAPHICS_RENDER_CORE_HPP
#define _FF_GRAPHICS_RENDER_CORE_HPP

#include <ff/processes/Process.hpp>
#include <ff/processes/ProcessManager.hpp>

#include <ff/messages/CmdHandler.hpp>
#include <ff/commands/render/RenderCmd.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff/events/dev/PopulateImGuiFrameEvent.hpp>

#include <ff/graphics/FullscreenRenderTarget.hpp>

#include <ff/graphics/PerspectSceneCamera.hpp>
#include <ff/directors/DebugCameraDirector.hpp>

#include <ff/render/RenderPipeline.hpp>

namespace ff {

class RenderCore final : public Process,
    public CmdHandler<RenderCmd>,
    public EventListener<PopulateImGuiFrameEvent>
{
public:
    RenderCore(ProcessManager* const& processManager);
    virtual ~RenderCore() = default;

    std::unique_ptr<RenderCmd::Ret> handleCmd(RenderCmd const& cmd) override;

    bool processEvent(PopulateImGuiFrameEvent const& event) override;

protected:
    void onInitialize() override;
    void onUpdate(const float& dt) override;

private:
    ProcessManager* _processManager;

    std::unique_ptr<FullscreenRenderTarget> _sceneGameCameraTarget;
    ResourceHandle<ColorTexture> _sceneGameCameraTargetColorTexture;

    std::shared_ptr<DebugCameraDirector> _debugCamDirector;

    std::unique_ptr<RenderPipeline> _renderPipeline;
    
    float _elapsed;

    void initRenderDetails();
};

}

#endif

