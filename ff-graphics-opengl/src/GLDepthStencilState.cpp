/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLDepthStencilState.hpp>

#include <ff-graphics-opengl/gl.hpp>
#include <ff/Console.hpp>

#include <ff-graphics-opengl/GLGlobalState.hpp>

namespace ff {
    GLenum convertDepthCompareFunctionToGLDepthCompareFunction(DepthCompareFunction depthCompareFunction) {
        switch(depthCompareFunction) {
            case DepthCompareFunction::NEVER:
                return GL_NEVER;
            case DepthCompareFunction::LESS:
                return GL_LESS;
            case DepthCompareFunction::EQUAL:
                return GL_EQUAL;
            case DepthCompareFunction::LESS_EQUAL:
                return GL_LEQUAL;
            case DepthCompareFunction::GREATER:
                return GL_GREATER;
            case DepthCompareFunction::GREATER_EQUAL:
                return GL_GEQUAL;
            case DepthCompareFunction::ALWAYS:
                return GL_ALWAYS;
            default:
                FF_CONSOLE_FATAL("Unknown conversion of depth compare function to OpenGL depth compare function.");
                return -1;
        }
    }

    GLDepthStencilState::GLDepthStencilState(GLGraphicsDevice* const& graphicsDevice,
        const DepthCompareFunction& depthCompareFunction,
        const bool& depthWriteEnabled)
        :_graphicsDevice(graphicsDevice),
        IDepthStencilState(depthCompareFunction, depthWriteEnabled) {
    }

    void GLDepthStencilState::bind() {
        _graphicsDevice->getGlobalState()->setDepthFunction(convertDepthCompareFunctionToGLDepthCompareFunction(getDepthCompareFunction()));
        _graphicsDevice->getGlobalState()->setDepthWriteEnabled(getDepthWriteEnabled());
        // If we have a depth stencil state, we have depth testing.
        _graphicsDevice->getGlobalState()->setDepthEnabled(true);
    }
}
