/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/render/RenderPipeline.hpp>

#include <ff/render/ConstantRenderNode.hpp>
#include <ff/render/CameraRenderNode.hpp>
#include <ff/render/ColorTextureSampleRenderNode.hpp>

#include <ff/Locator.hpp>
#include <ff/CVars.hpp>

#include <imgui.h>
#include <imnodes/ImNodesEz.h>

namespace ff {
    
void RenderPipeline::init() {
    Locator::getMessageBus().addListener<PopulateImGuiFrameEvent>(this);

    // @todo Temporary for testing
    attachNode(std::make_shared<IntConstantRenderNode>(0));
    attachNode(std::make_shared<CameraRenderNode>("gameplay"));
    attachNode(std::make_shared<ColorTextureSampleRenderNode>("null"));
}
void RenderPipeline::attachNode(std::shared_ptr<IRenderNode> node) {
    _nodes.push_back(node);
}

void RenderPipeline::render() {
    // Pre-render
    for(auto& node : _nodes) {
        node->prepare();
    }
}

bool RenderPipeline::processEvent(PopulateImGuiFrameEvent const& event) {
    if(CVars::get<bool>("debug_show_render_pipeline")) {
        ImGui::SetNextWindowSize(ImVec2(720, 480),
            ImGuiCond_FirstUseEver);
        if(ImGui::Begin("Render Pipeline Editor",
            &CVars::get<bool>("debug_show_render_pipeline"),
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
            ImNodes::Ez::BeginCanvas();

            /*struct Node {
                ImVec2 pos{};
                bool selected{};
                ImNodes::Ez::SlotInfo inputs[1];
                ImNodes::Ez::SlotInfo outputs[1];
            };

            static Node nodes[3] = {
                {{50, 100}, false, {{"In", 1}}, {{"Out", 1}}},
                {{250, 50}, false, {{"In", 1}}, {{"Out", 1}}},
                {{250, 100}, false, {{"In", 1}}, {{"Out", 1}}}
            };

            for(Node& node : nodes) {
                if(ImNodes::Ez::BeginNode(&node, "Node Title", &node.pos, &node.selected)) {
                    ImNodes::Ez::InputSlots(node.inputs, 1);
                    ImNodes::Ez::OutputSlots(node.outputs, 1);
                    ImNodes::Ez::EndNode();
                }
            }*/

            for(auto& node : _nodes) {
                node->drawImGuiNode();
            }

            ImNodes::Ez::EndCanvas();
        }
        ImGui::End();
    }

    return false;
}

}
