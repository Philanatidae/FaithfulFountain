/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_RENDER_RENDER_PIPELINE_HPP
#define _FF_RENDER_RENDER_PIPELINE_HPP

#include <memory>
#include <vector>

#include <ff/render/RenderNode.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff/events/dev/PopulateImGuiFrameEvent.hpp>

#include <imnodes/ImNodesEz.h>

namespace ff {

class RenderPipeline final : public EventListener<PopulateImGuiFrameEvent> {
public:
    ~RenderPipeline() = default;

    void init();
    void attachNode(std::shared_ptr<IRenderNode> node);

    void render();

    bool processEvent(PopulateImGuiFrameEvent const& event) override;

private:
    std::vector<std::shared_ptr<IRenderNode>> _nodes;

    //std::unique_ptr<ImNodes::CanvasState> _nodesCanvas; @TODO Add localized canvas for ImNodes
};

}

#endif

