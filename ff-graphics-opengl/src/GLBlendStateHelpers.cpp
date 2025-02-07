/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLBlendStateHelpers.hpp>

#include <ff/Console.hpp>

namespace ff {
    GLenum convertBlendOperationToGLBlendOperation(const BlendOperation& blendOperation) {
        switch(blendOperation) {
            case BlendOperation::Add:
                return GL_FUNC_ADD;
            case BlendOperation::Subtract:
                return GL_FUNC_SUBTRACT;
            case BlendOperation::ReverseSubtract:
                return GL_FUNC_REVERSE_SUBTRACT;
            case BlendOperation::Min:
                return GL_MIN;
            case BlendOperation::Max:
                return GL_MAX;

            default:
                FF_CONSOLE_ERROR("Unknown conversion of blend operation `%s` to OpenGL.", (int)blendOperation);
                return -1;
        }
    }
    BlendOperation convertGLBlendOperationToBlendOperation(const GLenum& blendOperation) {
        switch(blendOperation) {
            case GL_FUNC_ADD:
                return BlendOperation::Add;
            case GL_FUNC_SUBTRACT:
                return BlendOperation::Subtract;
            case GL_FUNC_REVERSE_SUBTRACT:
                return BlendOperation::ReverseSubtract;
            case GL_MIN:
                return BlendOperation::Min;
            case GL_MAX:
                return BlendOperation::Max;

            default:
                return BlendOperation::Invalid;
        }
    }

    GLenum convertBlendFactorToGLBlendFactor(const BlendFactor& blendFactor) {
        switch(blendFactor) {
            case BlendFactor::Zero:
                return GL_ZERO;
            case BlendFactor::One:
                return GL_ONE;
            
            case BlendFactor::Source:
                return GL_SRC_COLOR;
            case BlendFactor::OneMinusSource:
                return GL_ONE_MINUS_SRC_COLOR;

            case BlendFactor::SourceAlpha:
                return GL_SRC_ALPHA;
            case BlendFactor::OneMinusSourceAlpha:
                return GL_ONE_MINUS_SRC_ALPHA;

            case BlendFactor::Dest:
                return GL_DST_COLOR;
            case BlendFactor::OneMinusDest:
                return GL_ONE_MINUS_DST_COLOR;

            case BlendFactor::DestAlpha:
                return GL_DST_ALPHA;
            case BlendFactor::OneMinusDestAlpha:
                return GL_ONE_MINUS_DST_ALPHA;

            default:
                FF_CONSOLE_ERROR("Unknown conversion of blend factor `%s` to OpenGL.", (int)blendFactor);
                return -1;
        }
    }
    BlendFactor convertGLBlendFactorToBlendFactor(const GLenum& blendFactor) {
        switch(blendFactor) {
            case GL_ZERO:
                return BlendFactor::Zero;
            case GL_ONE:
                return BlendFactor::One;
            
            case GL_SRC_COLOR:
                return BlendFactor::Source;
            case GL_ONE_MINUS_SRC_COLOR:
                return BlendFactor::OneMinusSource;

            case GL_SRC_ALPHA:
                return BlendFactor::SourceAlpha;
            case GL_ONE_MINUS_SRC_ALPHA:
                return BlendFactor::OneMinusSourceAlpha;

            case GL_DST_COLOR:
                return BlendFactor::Dest;
            case GL_ONE_MINUS_DST_COLOR:
                return BlendFactor::OneMinusDest;

            case GL_DST_ALPHA:
                return BlendFactor::DestAlpha;
            case GL_ONE_MINUS_DST_ALPHA:
                return BlendFactor::OneMinusDestAlpha;

            default:
                return BlendFactor::Invalid;
        }
    }
}
