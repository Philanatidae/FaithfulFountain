/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_DEPTH_STENCIL_STATE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_DEPTH_STENCIL_STATE_HPP

#include <ff-graphics-opengl/gl.hpp>
#include <ff-graphics-opengl/GLGraphicsDevice.hpp>

#include <ff/graphics/IDepthStencilState.hpp>

namespace ff {
    GLenum convertDepthCompareFunctionToGLDepthCompareFunction(DepthCompareFunction depthCompareFunction);

    class GLDepthStencilState : public IDepthStencilState {
    public:
        GLDepthStencilState(GLGraphicsDevice* const& graphicsDevice,
            const DepthCompareFunction& depthCompareFunction,
            const bool& depthWriteEnabled);
        ~GLDepthStencilState() = default;

        void bind();

    private:
        GLGraphicsDevice* const _graphicsDevice;

        DepthCompareFunction _depthCompareFunction;
        bool _depthWriteEnabled;
    };
}

#endif
