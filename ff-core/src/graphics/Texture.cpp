/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/Texture.hpp>

#include <ff/Locator.hpp>

namespace ff {

ColorTexture* DefaultAssetLoader<ColorTexture>::load(IAssetBundle& assetBundle,
    const nlohmann::json& assetObject) {
    FF_ASSET_TYPE_CHECK(assetObject, "Texture");

    FF_ASSERT(!assetObject["path"].is_null(), "Missing key `path` in texture for asset `%s`.", assetObject["name"]);

    TextureFlag_t flags = 0;
    if(assetObject["mip-map"]) {
        //flags |= TextureFlags::GEN_MIP_MAPS;
        // @todo Mip maps _can_ be generated, using blitter. Texture
        // does not do this automatically anymore.
    }

    ResourceHandle<TextureData> data = assetBundle.load<TextureData>(assetObject["path"]);

    // @todo Remove CPU_WRITE, blit the texture into private GPU memory
    // instead.
    TextureUsage_t const usage = TextureUsage::GPU_SAMPLE
        | TextureUsage::GPU_BLIT
        | TextureUsage::CPU_WRITE;

    ColorTexture* texture = Locator::getGraphicsDevice().createRawColorTexture(convertTextureFormatToColorBufferFormat(data->getFormat()),
        data->getWidth(),
        data->getHeight(),
        flags,
        usage,
        (std::string)assetObject["name"]);
    texture->bufferImage(data);

    return texture;
}

}
