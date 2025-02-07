# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

target_sources(ff-graphics-opengl PRIVATE
    CVarDefaults.cpp
    gl.cpp
    GLBlendStateHelpers.cpp
    GLBlitPass.cpp
    GLBuffer.cpp
    GLBufferFormats.cpp
    GLDepthStencilState.cpp
    GLFramebuffer.cpp
    GLFramebufferManager.cpp
    GLGlobalState.cpp
    GLGraphicsDevice.cpp
    GLIndexBufferImp.cpp
    GLRenderPass.cpp
    GLRenderState.cpp
    GLSamplerState.cpp
    GLShader.cpp
    GLShaderFunction.cpp
    GLShaderLibrary.cpp
    GLTextureImp.cpp
    GLUniformBufferImp.cpp
    GLVertexBufferImp.cpp
    GLVertexDescriptor.cpp
)
