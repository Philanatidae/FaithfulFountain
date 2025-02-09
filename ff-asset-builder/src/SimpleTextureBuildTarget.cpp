/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/SimpleTextureBuildTarget.hpp>

#include <tinyformat/tinyformat.h>
#include <ff-asset-builder/TextureRegionBuildTarget.hpp>

#include <ff-asset-builder/AssetBuilder.hpp>

#include <ff/graphics/TextureData.hpp>
#include <ff/io/StreamBinaryReader.hpp>

#include <ff/util/OS.hpp>
#include <ff/math/MathHelper.hpp>

#include <fstream>

namespace ff {
    SimpleTextureBuildTarget::SimpleTextureBuildTarget(const std::string& name,
        const std::string& textureRegionName,
        const bool& isAlphaPremultiplied,
        const bool& mipMap)
        :_textureRegionName(textureRegionName),
        _name(name),
        _isAlphaPremultiplied(isAlphaPremultiplied),
        _mipMap(mipMap) {
    }
    SimpleTextureBuildTarget::~SimpleTextureBuildTarget() {
    }

    std::string SimpleTextureBuildTarget::getType() const {
        return "Texture";
    }
    std::string SimpleTextureBuildTarget::getName() const {
        return _name;
    }
    uint32_t SimpleTextureBuildTarget::getFlags() const {
        return 0;
    }

    std::vector<std::filesystem::path> SimpleTextureBuildTarget::getOutputs(AssetBuilder* builder) const {
        return {
            builder->getTargetDir()/_name
        };
    }

    const bool& SimpleTextureBuildTarget::isAlphaPremultiplied() const {
        return _isAlphaPremultiplied;
    }

    const std::string& SimpleTextureBuildTarget::getTextureRegionName() const {
        return _textureRegionName;
    }

    void SimpleTextureBuildTarget::build(AssetBuilder* builder) {
        FF_ASSERT(getInputs().size() == 1, "SimpleTextureBuildTarget must have exactly one input.");
        auto& sourcePath = getInputs()[0];

        // Collect some metadata for the texture region
        std::ifstream textureStream(sourcePath.string(), std::ifstream::binary);
        FF_ASSERT(textureStream.good(), "Could not open texture `%s`.", sourcePath);
        TextureData textureData(textureStream);
        int sx = 0;
        int sy = 0;
        int sw = textureData.getWidth();
        int sh = textureData.getHeight();

        // If we are to mip map, do an early check.
        if(_mipMap) {
            FF_ASSERT(MathHelper::isPowerOfTwo(textureData.getWidth()), "Texture width must be a power of two (width is %s).", textureData.getWidth()); 
            FF_ASSERT(MathHelper::isPowerOfTwo(textureData.getHeight()), "Texture height must be a power of two (height is %s).", textureData.getHeight()); 
        }

        copyFileIfNewer(sourcePath, getOutputs(builder)[0]);
    }
    void SimpleTextureBuildTarget::populateMetadata(nlohmann::json& targetObject) {
        targetObject["path"] = _name;
        targetObject["pre-multiplied-alpha"] = _isAlphaPremultiplied;
        targetObject["mip-map"] = _mipMap;
    }
}
