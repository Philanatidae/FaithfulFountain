/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/IBlitPass.hpp>

namespace ff {
    void IBlitPass::genMipMaps(ff::ResourceHandle<ColorTexture> const& texture) {
        FF_ASSERT((texture->getFlags() & TextureUsage::GPU_BLIT) > 0,
            "Texture is missing `GPU_BLIT` usage.");

        genMipMapsImpl(texture);
    }

    void IBlitPass::end() {
        endImpl();
    }
}
