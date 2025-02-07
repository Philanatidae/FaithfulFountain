/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/TextureAtlasBuildTarget.hpp>

#include <unordered_map>
#include <ff-asset-builder/TextureBuildSource.hpp>
#include <ff-asset-builder/TextureRegionBuildTarget.hpp>

#include <ff-asset-builder/AssetBuilder.hpp>

#include <ff/Console.hpp>
#include <ff/CVars.hpp>
#include <ff/util/String.hpp>
#include <ff/graphics/TextureData.hpp>

#include <stb_image_write.h>
#include <stb_rect_pack.h>

#include <fstream>

namespace ff {
    TextureAtlasBuildTarget::TextureAtlasBuildTarget(const std::string& name,
        const std::vector<std::string>& textureSourceNames,
        const int& borderSize,
        const int& padding,
        const bool& convertAllToPremultipliedAlpha)
        :_name(name),_textureSourceNames(textureSourceNames),
        _borderSize(borderSize),_padding(padding),
        _convertAllToPremultipliedAlpha(convertAllToPremultipliedAlpha),
        _isAlphaPremultiplied(false) {
    }
    TextureAtlasBuildTarget::~TextureAtlasBuildTarget() {
    }

    std::string TextureAtlasBuildTarget::getType() const {
        return "Texture";
    }
    std::string TextureAtlasBuildTarget::getName() const {
        return _name;
    }
    uint32_t TextureAtlasBuildTarget::getFlags() const {
        return 0;
    }

    std::vector<std::filesystem::path> TextureAtlasBuildTarget::getOutputs(AssetBuilder* builder) const {
        return {
            builder->getTargetDir()/_name
        };
    }

    const std::vector<std::string>& TextureAtlasBuildTarget::getTextureSourceNames() const {
        return _textureSourceNames;
    }
    const int& TextureAtlasBuildTarget::getBorderSize() const {
        return _borderSize;
    }
    const int& TextureAtlasBuildTarget::getPadding() const {
        return _padding;
    }

    void TextureAtlasBuildTarget::build(AssetBuilder* builder) {
        if(_textureSourceNames.size() == 0) {
            FF_CONSOLE_ERROR("Texture atlas `%s` has no texture sources.", _name);
            return;
        }

        // Collect texture build sources
        std::unordered_map<std::string, std::shared_ptr<TextureBuildSource>> textureBuildSources;
        for (const auto& textureSourceName : _textureSourceNames) {
            FF_ASSERT(builder->getBuildSources().find(textureSourceName) != builder->getBuildSources().end(),
                "Build source '%s' not found. It either does not exist or its usage is not `source`.", textureSourceName);
            std::shared_ptr<BuildSource> buildSource = builder->getBuildSources().at(textureSourceName);
            textureBuildSources.emplace(textureSourceName, std::static_pointer_cast<TextureBuildSource>(buildSource));
        }

        // Read in texture data
        // @todo Should this go in the source? Maybe some sort of caching mechanism?
        std::unordered_map<std::string, std::unique_ptr<TextureData>> textureData;
        for (const auto& textureSourceName : _textureSourceNames) {
            std::shared_ptr<TextureBuildSource> textureBuildSource = textureBuildSources.at(textureSourceName);
            std::string textureSourcePath = textureBuildSource->getInputs()[0].string();
            std::ifstream textureStream(textureSourcePath, std::ios::binary);
            FF_ASSERT(textureStream.is_open(), "Failed to open texture '%s'.", textureSourcePath);
            std::unique_ptr<TextureData> textureDataPtr = std::make_unique<TextureData>(textureStream);
            textureData.emplace(textureSourceName, std::move(textureDataPtr));
        }

        // Create texture atlas
        // @todo Add texture creation/saving features to ff-core
        if(_convertAllToPremultipliedAlpha) {
            FF_CONSOLE_LOG("Textures used in the texture atlas '%s' will be converted to premultiplied alpha.", _name);
        }

        // Interesting and imformative reading on rectangle packing algorithms: https://www.david-colson.com/2020/03/10/exploring-rect-packing.html
        // We'll just use stb_rect_pack since it's easy to implement and does a fairly decent job.

        // Sum together the areas of all the textures to appriximate the total area needed for the atlas.
        int totalArea = 0;
        for(auto& textureDataPair : textureData) {
            const auto& textureData = textureDataPair.second;
            totalArea += (textureData->getWidth() + 2 * getPadding())
                * (textureData->getHeight() + 2 * getPadding());
        }

        // Find the closest power of two that is greater than or equal to the total area.
        int atlasWidth = 1;
        int atlasHeight = 1;
        while((atlasWidth - 2 * getBorderSize()) * (atlasHeight -  2 * getBorderSize()) < totalArea) {
            atlasWidth <<= 1;
            atlasHeight <<= 1;
        }
        
        // Pack the textures into the atlas.
        std::vector<stbrp_rect> rects;
        rects.reserve(textureData.size());
        for(int i = 0; i < textureData.size(); i++) {
            const auto& texDat = textureData.at(_textureSourceNames[i]);
            stbrp_rect rect{};
            rect.id = i; // Map id to index in the texture data vector.
            rect.w = texDat->getWidth() + 2 * getPadding();
            rect.h = texDat->getHeight() + 2 * getPadding();
            rects.push_back(rect);
        }

        // Attempt to pack the textures into the atlas.
        // This will keep increasing the atlas size to the closest power of two until it fits.
        bool wereRectsPacked = false;
        while(!wereRectsPacked) {
            std::vector<stbrp_node> nodes(atlasWidth);
            stbrp_context context;
            stbrp_init_target(&context, atlasWidth - getBorderSize() * 2, atlasHeight - getBorderSize() * 2, nodes.data(), nodes.size());

            wereRectsPacked = stbrp_pack_rects(&context, rects.data(), rects.size());

            if(!wereRectsPacked) {
                atlasWidth <<= 1;
                atlasHeight <<= 1;
                continue;
            }
        }
        FF_ASSERT(atlasWidth <= CVars::get<float>("asset_builder_atlas_maximum_extent")
            && atlasHeight <= CVars::get<float>("asset_builder_atlas_maximum_extent"),
            "Atlas `%s` could not be generated within the maximum extent allowed (w: %s, h: %s, maximum: %s). Multiple atlases may need to be used or sources may need to be smaller.",
            _name, atlasWidth, atlasHeight, CVars::get<float>("asset_builder_atlas_maximum_extent"));
        float atlasUsage = totalArea / (float)(atlasWidth * atlasHeight);

        // Determine if the atlas is alpha premultiplied.
        if(_convertAllToPremultipliedAlpha) {
            // Convert non-premultiplied alpha to premultiplied alpha
            _isAlphaPremultiplied = true;
        } else {
            // Initial state; get first texture pre-multiplied flag.
            _isAlphaPremultiplied = textureData.at(_textureSourceNames[0])->isPreMultipliedAlpha();
            // Loop through the texture sources. If any differ from,
            // the first texture, then we have to abort since the
            // convertAllToPremultipliedAlpha flag is not set.
            for(int i = 1; i < textureData.size(); i++) {
                FF_ASSERT(textureData.at(_textureSourceNames[i])->isPreMultipliedAlpha() == _isAlphaPremultiplied,
                    "All textures used in the texture atlas '%s' must have the same premultiplied alpha flag.", _name);
            }
        }

        // Create the atlas texture
        std::vector<uint8_t> atlasData(atlasWidth * atlasHeight * 4);
        memset(&atlasData[0], 0, atlasData.size());
        for(int i = 0; i < rects.size(); i++) {
            const auto& rect = rects[i];
            const auto& texDat = textureData.at(_textureSourceNames[i]);
            const int x = rect.x + getPadding() + getBorderSize();
            const int y = rect.y + getPadding() + getBorderSize();
            const int w = texDat->getWidth();
            const int h = texDat->getHeight();
            for(int yy = 0; yy < h; yy++) {
                for(int xx = 0; xx < w; xx++) {
                    const int atlasIndex = ((y + yy) * atlasWidth + x + xx) * 4;
                    const int textureIndex = (yy * texDat->getWidth() + xx) * 4;
                    atlasData[atlasIndex + 0] = ((uint8_t*)texDat->getData())[textureIndex + 0];
                    atlasData[atlasIndex + 1] = ((uint8_t*)texDat->getData())[textureIndex + 1];
                    atlasData[atlasIndex + 2] = ((uint8_t*)texDat->getData())[textureIndex + 2];
                    atlasData[atlasIndex + 3] = ((uint8_t*)texDat->getData())[textureIndex + 3];
                    // If _computeAllToPremultipliedAlpha is true, then we'll multiply alpha
                    // onto the color values.
                    if(_convertAllToPremultipliedAlpha) {
                        atlasData[atlasIndex + 0] = (uint8_t)((float)atlasData[atlasIndex + 0] * atlasData[atlasIndex + 3] / 255.0f);
                        atlasData[atlasIndex + 1] = (uint8_t)((float)atlasData[atlasIndex + 1] * atlasData[atlasIndex + 3] / 255.0f);
                        atlasData[atlasIndex + 2] = (uint8_t)((float)atlasData[atlasIndex + 2] * atlasData[atlasIndex + 3] / 255.0f);
                    }
                }
            }
            // Add texture region as a build target now that it has been placed in the atlas.
            auto target = builder->addBuildTarget(_textureSourceNames[i],
                std::make_shared<TextureRegionBuildTarget>(
                    _textureSourceNames[i],
                    _name,
                    x, y,
                    w, h));
            builder->addTargetToBuild(_textureSourceNames[i]);
            addProduct(target->getName());
        }

        // Save atlas to file as PNG using stb_image_write.
        FF_CONSOLE_LOG("Writing Texture Atlas '%s' (width: %s, height: %s, texture count: %s, space utilization: %s%%)...", _name,
            atlasWidth, atlasHeight,
            textureData.size(),
            atlasUsage * 100.0f);
        stbi_write_png(getOutputs(builder)[0].string().c_str(), atlasWidth, atlasHeight, 4, &atlasData[0], atlasWidth * 4);
    }
    void TextureAtlasBuildTarget::populateMetadata(nlohmann::json& targetObject) {
        targetObject["path"] = _name;
        targetObject["pre-multiplied-alpha"] = _isAlphaPremultiplied;
    }
}
