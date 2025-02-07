/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_FRAMEBUFFER_MANAGER_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_FRAMEBUFFER_MANAGER_HPP

#include <ff-graphics-opengl/gl.hpp>
#include <ff-graphics-opengl/GLGraphicsDevice.hpp>
#include <ff/graphics/Texture.hpp>
#include <ff/graphics/IRenderPass.hpp>
#include <ff-graphics-opengl/GLFramebuffer.hpp>
#include <ff-graphics-opengl/GLTextureImp.hpp>

#include <optional>
#include <ff/graphics/BlendState.hpp>

#include <array>

#include <stdint.h>

namespace ff {

class GLFramebuffer;

class GLFramebufferManager {
public:
    GLFramebufferManager(GLGraphicsDevice* const& graphicsDevice);
    ~GLFramebufferManager() = default;

    void init();

    GLFramebuffer* prepareForDraw(std::vector<std::pair<ResourceHandle<ColorTexture>,
            RenderTargetColorAttachmentOptions>> const& colorTargets,
        std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget);
    void attemptInvalidation(std::vector<std::pair<ResourceHandle<ColorTexture>,
            RenderTargetColorAttachmentOptions>> const& colorTargets,
        std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget);
    void prepareForCPUTransfer(ResourceHandle<ITexture> const& tex);
    void prepareForCPUTransfer(GLTextureImp* const& glTexImp);

    void prepareForBlitTransfer(ResourceHandle<ITexture> const& src,
        ResourceHandle<ITexture> const& dst);

    void setDrawFBOBlendState(int const& slot, std::optional<BlendState> const& blendState);

private:
    GLGraphicsDevice* const _graphicsDevice;

    std::unique_ptr<GLFramebuffer> _drawFBO;
    std::array<GLenum, 4> _drawFBOSlotToAttachment;
    std::unique_ptr<GLFramebuffer> _readFBO;
};

}

#endif

