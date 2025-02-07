/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_BLEND_STATE_HELPERS_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_BLEND_STATE_HELPERS_HPP

#include <ff/graphics/BlendState.hpp>
#include <ff-graphics-opengl/gl.hpp>

namespace ff {
    GLenum convertBlendOperationToGLBlendOperation(const BlendOperation& blendOperation);
    BlendOperation convertGLBlendOperationToBlendOperation(const GLenum& blendOperation);

    GLenum convertBlendFactorToGLBlendFactor(const BlendFactor& blendFactor);
    BlendFactor convertGLBlendFactorToBlendFactor(const GLenum& blendFactor);
}

#endif
