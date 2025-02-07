/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_DEFERRED_BLIT_PASS_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_DEFERRED_BLIT_PASS_HPP

#include <ff/graphics/IBlitPass.hpp>

#include <functional>
#include <vector>

namespace ff {

class DeferredBlitPass : public IBlitPass {    
public:
    DeferredBlitPass();

    void encode(IBlitPass* const& blitPass);

protected:
    void genMipMapsImpl(ResourceHandle<ColorTexture> const& texture) override;

    void endImpl() override;

private:
    std::vector<std::function<void(IBlitPass* const&)>> _encodingFns;
    bool _locked;
};

}

#endif
