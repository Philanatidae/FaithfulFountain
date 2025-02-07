/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_RENDER_RENDER_NODE_HPP
#define _FF_RENDER_RENDER_NODE_HPP

#include <ff/render/RenderNodeIO.hpp>
#include <array>
#include <ff/Console.hpp>

#include <imgui.h>
#include <imnodes/ImNodesEz.h>

#include <vector>

namespace ff {

struct IRenderNode;

struct RenderNodeInput {
    RenderNodeIOType type = RenderNodeIOType::INT;
    std::string name;
    bool optional = false;

    IRenderNode* node = nullptr;
    int nodeOutputIdx = -1;
};

struct IRenderNode {
    friend class RenderPipeline;

    virtual ~IRenderNode() = default;

    virtual void prepare() = 0;

    virtual void process(float const& alpha) = 0;

    virtual int getInputCount() const = 0;
    virtual RenderNodeIOType getInputType(int const& idx) const = 0;
    virtual bool isInputOptional(int const& idx) const = 0;
    virtual void setInput(int const& idx,
        IRenderNode* node, int nodeOutputIdx) = 0;
    virtual void clearInput(int const& idx) = 0;
    virtual IRenderNodeIO const* getInput(int const& idx) const = 0;

    virtual int getOutputCount() const = 0;
    virtual std::unique_ptr<IRenderNodeIO>& getOutput(int const& idx) = 0;
    virtual IRenderNodeIO const* getOutput(int const& idx) const = 0;

    virtual void drawImGuiNode() = 0;
};
template<int InputCount, int OutputCount>
struct RenderNode : public IRenderNode {
    RenderNode(std::string const& name)
        :_name(name),
        _imguiPos(0, 0),
        _imguiSelected(false) {
    }
    virtual ~RenderNode() = default;

    int getInputCount() const override {
        return InputCount;
    }
    RenderNodeIOType getInputType(int const& idx) const override {
        FF_ASSERT(idx < _inputs.size() && idx > -1, "Invalid index %s on range [0, %s]",
            idx, InputCount - 1);
        return _inputs[idx].type;
    }
    bool isInputOptional(int const& idx) const override {
        FF_ASSERT(idx < _inputs.size() && idx > -1, "Invalid index %s on range [0, %s]",
            idx, InputCount - 1);
        return _inputs[idx].optional;
    }
    void setInput(int const& idx,
        IRenderNode* node, int nodeOutputIdx) override {
        FF_ASSERT(idx < _inputs.size() && idx > -1, "Invalid index %s on range [0, %s]",
            idx, InputCount - 1);
        FF_ASSERT(node != nullptr, "Node cannot be null");
        _inputs[idx].node = node;
        _inputs[idx].nodeOutputIdx = nodeOutputIdx;
    }
    void clearInput(int const& idx) override {
        FF_ASSERT(idx < _inputs.size() && idx > -1, "Invalid index %s on range [0, %s]",
            idx, InputCount - 1);
        _inputs[idx].node = nullptr;
        _inputs[idx].nodeOutputIdx = -1;
    }
    IRenderNodeIO const* getInput(int const& idx) const override {
        FF_ASSERT(idx < _inputs.size() && idx > -1, "Invalid index %s on range [0, %s]",
            idx, InputCount - 1);
        if(_inputs[idx].node == nullptr) {
            return nullptr;
        }
        return _inputs[idx].node->getOutput(_inputs[idx].nodeOutputIdx).get();
    }

    int getOutputCount() const override {
        return OutputCount;
    }
    std::unique_ptr<IRenderNodeIO>& getOutput(int const& idx) override {
        FF_ASSERT(idx < _outputs.size() && idx > -1, "Invalid index %s on range [0, %s]",
            idx, OutputCount - 1);
        return _outputs[idx];
    }
    IRenderNodeIO const* getOutput(int const& idx) const override {
        FF_ASSERT(idx < _outputs.size() && idx > -1, "Invalid index %s on range [0, %s]",
            idx, OutputCount - 1);
        return _outputs[idx].get();
    }

    void drawImGuiNode() override {
        if(ImNodes::Ez::BeginNode(this,
            _name.c_str(),
            &_imguiPos,
            &_imguiSelected)) {
            // Note that ImNodes cares about the order
            // of how node contents is defined.
            // 1) Input
            // 2) Content
            // 3) Output
            // 4) Connections
            
            std::vector<ImNodes::Ez::SlotInfo> inputSlots;
            for(int i = 0; i < _inputs.size(); i++) {
                inputSlots.push_back(ImNodes::Ez::SlotInfo{
                    _inputs[i].name.c_str(),
                    (int)_inputs[i].type
                });
            }
            ImNodes::Ez::InputSlots(inputSlots.size() > 0 ? inputSlots.data() : nullptr,
                inputSlots.size());

            drawImGuiNodeContents();

            std::vector<ImNodes::Ez::SlotInfo> outputSlots;
            for(int i = 0; i < _outputs.size(); i++) {
                outputSlots.push_back(ImNodes::Ez::SlotInfo{
                    _outputs[i]->getName(),
                    (int)_outputs[i]->getType()
                });
            }
            ImNodes::Ez::OutputSlots(outputSlots.size() > 0 ? outputSlots.data() : nullptr, outputSlots.size());

            ImNodes::Ez::EndNode();
        }
    }

protected:
    std::array<RenderNodeInput, InputCount> _inputs;
    std::array<std::unique_ptr<IRenderNodeIO>, OutputCount> _outputs;

    std::string _name;

    ImVec2 _imguiPos;
    bool _imguiSelected;

    virtual void drawImGuiNodeContents() = 0;
};

}

#endif

