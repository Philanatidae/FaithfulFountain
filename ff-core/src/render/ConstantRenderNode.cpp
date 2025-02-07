/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/render/ConstantRenderNode.hpp>

#include <glm/gtc/type_ptr.hpp>

namespace ff {

template<>
void ConstantRenderNode<RenderNodeIOType::INT, int>::drawImGuiNodeContents() {
    int val = getValue();
    ImGui::PushItemWidth(100);
    if(ImGui::InputInt("##value", &val)) {
        setValue(val);
    }
    ImGui::PopItemWidth();
}
template<>
void ConstantRenderNode<RenderNodeIOType::FLOAT, float>::drawImGuiNodeContents() {
    float val = getValue();
    if(ImGui::InputFloat("##value", &val)) {
        setValue(val);
    }
}
template<>
void ConstantRenderNode<RenderNodeIOType::VEC2, glm::vec2>::drawImGuiNodeContents() {
    glm::vec2 val = getValue();
    if(ImGui::InputFloat2("##value", &val.x)) {
        setValue(val);
    }
}
template<>
void ConstantRenderNode<RenderNodeIOType::VEC3, glm::vec3>::drawImGuiNodeContents() {
    glm::vec3 val = getValue();
    if(ImGui::InputFloat3("##value", &val.x)) {
        setValue(val);
    }
}
template<>
void ConstantRenderNode<RenderNodeIOType::VEC4, glm::vec4>::drawImGuiNodeContents() {
    glm::vec4 val = getValue();
    if(ImGui::InputFloat4("##value", &val.x)) {
        setValue(val);
    }
}
template<>
void ConstantRenderNode<RenderNodeIOType::MATRIX, glm::mat4x4>::drawImGuiNodeContents() {
    // Transpose, because GLM stores matrices internal as columns
    glm::mat4x4 val = glm::transpose(getValue());
    float const* valPtr = glm::value_ptr(val);

    glm::vec4 row1(valPtr[0], valPtr[1], valPtr[2], valPtr[3]);
    glm::vec4 row2(valPtr[4], valPtr[5], valPtr[6], valPtr[7]);
    glm::vec4 row3(valPtr[8], valPtr[9], valPtr[10], valPtr[11]);
    glm::vec4 row4(valPtr[12], valPtr[13], valPtr[14], valPtr[15]);

    if(ImGui::InputFloat4("##row1", &row1.x)
        || ImGui::InputFloat4("##row2", &row2.x)
        || ImGui::InputFloat4("##row3", &row3.x)
        || ImGui::InputFloat4("##row4", &row4.x)) {
        val = glm::mat4x4(row1, row2, row3, row4);
        // Transpose back
        setValue(glm::transpose(val));
    }
}
template<>
void ConstantRenderNode<RenderNodeIOType::COLOR, Color>::drawImGuiNodeContents() {
    Color val = getValue();
    if(ImGui::ColorEdit4("##value", val.data())) {
        setValue(val);
    }
}
template<>
void ConstantRenderNode<RenderNodeIOType::BOOL, bool>::drawImGuiNodeContents() {
    bool val = getValue();
    if(ImGui::Checkbox("##value", &val)) {
        setValue(val);
    }
}

}

