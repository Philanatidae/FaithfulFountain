/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLBlitPass.hpp>

#include <ff-graphics-opengl/GLTextureImp.hpp>

#include <ff-graphics-opengl/gl.hpp>

namespace ff {

GLBlitPass::GLBlitPass(GLGraphicsDevice* const& graphicsDevice)
    :_graphicsDevice(graphicsDevice) {
}

void GLBlitPass::genMipMapsImpl(ResourceHandle<ColorTexture> const& texture) {
    GLTextureImp* const glTexImp = static_cast<GLTextureImp*>(texture->getImp());
    glTexImp->genMipMaps();
}

void GLBlitPass::endImpl() {
    // @todo We can probably glFlush here, as long as we're
    // sure that the driver won't under any circumstance
    // block the thread as if we called glFinish.
}

}
