/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLSamplerState.hpp>

#include <ff/Console.hpp>

#include <ff-graphics-opengl/GLTextureImp.hpp>

namespace ff {
    GLenum convertFilterToGLFilter(const SamplerFilter& filter, const SamplerMipFilter& mipFilter) {
        switch(filter) {
            case SamplerFilter::NEAREST:
                switch(mipFilter) {
                case SamplerMipFilter::INVALID:
                    return GL_NEAREST;
                case SamplerMipFilter::LINEAR:
                    return GL_NEAREST_MIPMAP_LINEAR;
                case SamplerMipFilter::NEAREST:
                    return GL_NEAREST_MIPMAP_NEAREST;
                default:
                    FF_CONSOLE_ERROR("Unknown conversion of mip filter to OpenGL mip filter.");
                }
            case SamplerFilter::LINEAR:
                switch(mipFilter) {
                case SamplerMipFilter::INVALID:
                    return GL_LINEAR;
                case SamplerMipFilter::LINEAR:
                    return GL_LINEAR_MIPMAP_LINEAR;
                case SamplerMipFilter::NEAREST:
                    return GL_LINEAR_MIPMAP_NEAREST;
                default:
                    FF_CONSOLE_ERROR("Unknown conversion of mip filter to OpenGL mip filter.");
                }
            default:
                FF_CONSOLE_ERROR("Unknown conversion of sampler filter to OpenGL sampler filter.");
        }
    }
    GLenum convertAddressModeToGLAddressMode(const SamplerAddressMode& addressMode) {
        switch(addressMode) {
            case SamplerAddressMode::REPEAT_MIRROR:
                return GL_MIRRORED_REPEAT;
            case SamplerAddressMode::REPEAT:
                return GL_REPEAT;
            case SamplerAddressMode::CLAMP_TO_EDGE:
                return GL_CLAMP_TO_EDGE;
            default:
                FF_CONSOLE_ERROR("Unknown conversion of sampler address mode to OpenGL sampler address mode.");
        }
    }

    GLSamplerState::GLSamplerState(const SamplerFilter& minFilter,
        const SamplerFilter& magFilter,
        const SamplerAddressMode& sAddressMode,
        const SamplerAddressMode& tAddressMode,
        const SamplerMipFilter& mipFilter)
        :ISamplerState(minFilter, magFilter, sAddressMode, tAddressMode, mipFilter) {
    }

    void GLSamplerState::bindToTexture(const ResourceHandle<ColorTexture>& texture) {
        (static_cast<GLTextureImp*>(texture->getImp()))->setTexParams(convertFilterToGLFilter(getMinFilter(), getMipFilter()),
            convertFilterToGLFilter(getMagFilter(), SamplerMipFilter::INVALID),
            convertAddressModeToGLAddressMode(getSAddressMode()),
            convertAddressModeToGLAddressMode(getTAddressMode()));
    }
}
