/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_RENDER_CONSTANT_RENDER_NODE_HPP
#define _FF_RENDER_CONSTANT_RENDER_NODE_HPP

#include <ff/render/RenderNode.hpp>
#include <ff/render/RenderNodeIO.hpp>

#include <memory>

#include <tinyformat/tinyformat.h>

namespace ff {
    
template<RenderNodeIOType IOT, typename UT>
struct ConstantRenderNode : public RenderNode<0, 1> {
    static constexpr int OUTPUT_CONSTANT = 0;

    ConstantRenderNode(UT const& val)
        :RenderNode(tinyformat::format("%s Constant", convertRenderNodeIOTypeToFriendlyString(IOT))) {
        getOutput(OUTPUT_CONSTANT) = std::make_unique<RenderNodeIO<IOT, UT>>("constant", val);
    }

    void setValue(UT const& val) {
        ((RenderNodeIO<IOT, UT>*)getOutput(OUTPUT_CONSTANT).get())->setValue(val);
    }
    UT getValue() {
        return ((RenderNodeIO<IOT, UT>*)getOutput(OUTPUT_CONSTANT).get())->getValue();
    }

    void prepare() override {}
    void process(float const& alpha) override {}

protected:
    void drawImGuiNodeContents() override;
};

template<>
void ConstantRenderNode<RenderNodeIOType::INT, int>::drawImGuiNodeContents();
template<>
void ConstantRenderNode<RenderNodeIOType::FLOAT, float>::drawImGuiNodeContents();
template<>
void ConstantRenderNode<RenderNodeIOType::VEC2, glm::vec2>::drawImGuiNodeContents();
template<>
void ConstantRenderNode<RenderNodeIOType::VEC3, glm::vec3>::drawImGuiNodeContents();
template<>
void ConstantRenderNode<RenderNodeIOType::VEC4, glm::vec4>::drawImGuiNodeContents();
template<>
void ConstantRenderNode<RenderNodeIOType::MATRIX, glm::mat4x4>::drawImGuiNodeContents();
template<>
void ConstantRenderNode<RenderNodeIOType::COLOR, Color>::drawImGuiNodeContents();
template<>
void ConstantRenderNode<RenderNodeIOType::BOOL, bool>::drawImGuiNodeContents();

using IntConstantRenderNode = ConstantRenderNode<RenderNodeIOType::INT, int>;
using FloatConstantRenderNode = ConstantRenderNode<RenderNodeIOType::FLOAT, float>;
using Vec2ConstantRenderNode = ConstantRenderNode<RenderNodeIOType::VEC2, glm::vec2>;
using Vec3ConstantRenderNode = ConstantRenderNode<RenderNodeIOType::VEC3, glm::vec3>;
using Vec4ConstantRenderNode = ConstantRenderNode<RenderNodeIOType::VEC4, glm::vec4>;
using MatrixConstantRenderNode = ConstantRenderNode<RenderNodeIOType::MATRIX, glm::mat4x4>;
using ColorConstantRenderNode = ConstantRenderNode<RenderNodeIOType::COLOR, Color>;
using BoolRenderRenderNode = ConstantRenderNode<RenderNodeIOType::BOOL, bool>;

}

#endif

