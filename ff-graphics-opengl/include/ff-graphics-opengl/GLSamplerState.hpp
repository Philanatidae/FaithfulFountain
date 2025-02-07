/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_SAMPLER_STATE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_SAMPLER_STATE_HPP

#include <ff-graphics-opengl/gl.hpp>

#include <ff/graphics/ISamplerState.hpp>
#include <ff/resources/ResourceHandle.hpp>

#include <ff-graphics-opengl/GLTextureImp.hpp>

#include <memory>

namespace ff {
    GLenum convertFilterToGLFilter(const SamplerFilter& filter, const SamplerMipFilter& mipFilter);
    GLenum convertAddressModeToGLAddressMode(const SamplerAddressMode& addressMode);

    class GLSamplerState : public ISamplerState {
    public:
        GLSamplerState(const SamplerFilter& minFilter,
            const SamplerFilter& magFilter,
            const SamplerAddressMode& sAddressMode,
            const SamplerAddressMode& tAddressMode,
            const SamplerMipFilter& mipFilter);
        ~GLSamplerState() = default;

        void bindToTexture(const ResourceHandle<ColorTexture>& texture);
    private:
    };
}

#endif
