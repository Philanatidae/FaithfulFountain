/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/render/ColorTextureSampleRenderNode.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ff/Locator.hpp>

namespace ff {

ColorTextureSampleRenderNode::ColorTextureSampleRenderNode(std::string const& texture)
    :RenderNode("Color Texture (Sample)"),_texture(texture) {
    getOutput(OUTPUT_COLOR_TEXTURE) = std::make_unique<ColorTextureSampleRenderNodeIO>("texture", _texture);
}

void ColorTextureSampleRenderNode::setTexture(std::string const& texture) {
    _texture = texture;
}
std::string ColorTextureSampleRenderNode::getTexture() const {
    return _texture;
}

void ColorTextureSampleRenderNode::prepare() {
}
void ColorTextureSampleRenderNode::process(float const& alpha) {
    ((ColorTextureSampleRenderNodeIO*)getOutput(OUTPUT_COLOR_TEXTURE).get())->setValue(_texture);
}

void ColorTextureSampleRenderNode::drawImGuiNodeContents() {
    int const itemWidth = 200;

    ImGui::PushItemWidth(itemWidth);

    std::vector<std::string> textures = Locator::getColorTextureManager().getTextures();
    if(ImGui::BeginCombo("Color Texture", _texture.c_str())) {
        for(int i = 0; i < textures.size(); i++) {
            // Only textures that can be sampled are available to
            // be used as inputs
            auto texHandle = Locator::getColorTextureManager().getTexture(textures[i]);
            if((texHandle->getUsage() & TextureUsage::GPU_SAMPLE) == 0) {
                continue;
            }

            bool isSelected = textures[i] == _texture;
            if(ImGui::Selectable(textures[i].c_str(),
                isSelected)) {
                setTexture(textures[i]);
            }

            if(isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Texture preview (only when a texture has `GPU_SAMPLE` for now)
    auto textureHandle = Locator::getColorTextureManager().getTexture(_texture);
    if(textureHandle != nullptr) {
        if((textureHandle->getUsage() & TextureUsage::GPU_SAMPLE) != 0) {
            ImTextureID const texId = Locator::getColorTextureManager().getTextureAsImTextureID(_texture);
            float const aspectRatio = textureHandle->getAspectRatio();
            ImVec2 texSize(itemWidth, itemWidth / aspectRatio);
            ImGui::Image(texId, texSize);
        }
    }
}
    
}

