/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/DeferredBlitPass.hpp>

namespace ff {

DeferredBlitPass::DeferredBlitPass()
    :_locked(false) {
}

void DeferredBlitPass::encode(IBlitPass* const& blitPass) {
    for(auto fn : _encodingFns) {
        fn(blitPass);
    }
    _encodingFns.clear();
}

void DeferredBlitPass::genMipMapsImpl(ResourceHandle<ColorTexture> const& texture) {
    FF_ASSERT(!_locked, "Blit pass is locked (`end` was called).");

    _encodingFns.push_back([texture](IBlitPass* const& pass) -> void {
        pass->genMipMaps(texture);
    });
}

void DeferredBlitPass::endImpl() {
    _locked = true;
}
    
}
