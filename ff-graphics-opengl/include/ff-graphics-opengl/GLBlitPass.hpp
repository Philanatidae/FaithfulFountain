/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GLBLIT_PASS_HPP
#define GLBLIT_PASS_HPP

#include <ff-graphics-opengl/GLGraphicsDevice.hpp>

namespace ff {

class GLBlitPass : public IBlitPass {
public:
    GLBlitPass(GLGraphicsDevice* const& graphicsDevice);
    virtual ~GLBlitPass() = default;

protected:
    void genMipMapsImpl(ResourceHandle<ColorTexture> const& texture) override;

    void endImpl() override;

private:
    GLGraphicsDevice* const _graphicsDevice;
};

}

#endif
