# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

target_sources(ff-core PRIVATE
    BitmapFont.cpp
    BufferFormats.cpp
    Camera.cpp
    CameraManager.cpp
    ClearColor.cpp
    Color.cpp
    FullscreenRenderTarget.cpp
    DeferredBlitPass.cpp
    GPUInfo.cpp
    IBlitPass.cpp
    IdentityTextureSource.cpp
    IDepthStencilState.cpp
    IGraphicsDevice.cpp
    IndexBuffer.cpp
    IRenderPass.cpp
    IRenderState.cpp
    ISamplerState.cpp
    IVertexDescriptor.cpp
    Material.cpp
    ModelData.cpp
    NullTextureSource.cpp
    OrthographicCamera.cpp
    OrthoSceneCamera.cpp
    PerspectiveCamera.cpp
    PPQuad.cpp
    RenderCore.cpp
    ShaderFunctionBindingType.cpp
    Texture.cpp
    TextureData.cpp
    TextureRegion.cpp
    UniformBuffer.cpp
    VertexBuffer.cpp
)
