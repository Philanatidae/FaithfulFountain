/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_IBLIT_PASS_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_IBLIT_PASS_HPP

#include <ff/resources/ResourceHandle.hpp>
#include <ff/graphics/Texture.hpp>

namespace ff {

class IBlitPass {
public:
    virtual ~IBlitPass() = default;

    void genMipMaps(ff::ResourceHandle<ColorTexture> const& texture);

    void end();

protected:
    virtual void genMipMapsImpl(ResourceHandle<ColorTexture> const& texture) = 0;

    virtual void endImpl() = 0;
};

class NullBlitPass : public IBlitPass {
public:
    NullBlitPass() {}

    void genMipMapsImpl(ResourceHandle<ColorTexture> const& texture) override {
    }

    void endImpl() override {
    }
};

}

#endif
