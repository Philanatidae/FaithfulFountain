/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_GRAPHICS_DEVICE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_GRAPHICS_DEVICE_HPP

#include <ff/graphics/IGraphicsDevice.hpp>

#include <ff-graphics-opengl/gl.hpp>

#include <memory>

#include <glm/glm.hpp>

namespace ff {
    class GLShaderLibrary;
    class GLGlobalState;
    class GLFramebufferManager;
    class FullscreenRenderTarget;
    class PPQuad;

    using GLExtensions_t = uint64_t;
    namespace GLExtensions {
        constexpr GLExtensions_t ARB_DRAW_BUFFERS_BLEND = 1 << 0; // Core since GL 4.0, GLES 3.2
        constexpr GLExtensions_t ARB_TEXTURE_STORAGE = 1 << 1; // Core since GL 4.2, GLES 3.0
        constexpr GLExtensions_t ARB_INVALIDATE_SUBDATA = 1 << 2; // Core since GL 4.3, GLES 3.0
        constexpr GLExtensions_t ARB_DEBUG_LABEL = 1 << 3; // Core since GLES 3.2, not available until GL 4.3 Core. No extensions exist <4.3 Core.
        constexpr GLExtensions_t KHR_DEBUG = 1 << 4; // Extension alternative for glOutputLabel, use ARB_DEBG_LABEL when available.
    }

    class GLGraphicsDevice final : public IGraphicsDevice {
    public:
        GLGraphicsDevice(const GLenum& presentationColorBufferFormat,
            const GLenum& presentationDepthBufferFormat);
        ~GLGraphicsDevice();

        void initializeBackBuffer(const glm::ivec2& backBufferDimensions);

        GLGlobalState* getGlobalState();
        GLFramebufferManager* getFramebufferManager();

        char const* getGLVendor() const;
        char const* getGLRenderer() const;
        char const* getGLVersion() const;
        bool isGLES() const;
        int getMaxTextureUnits() const;
        int getMaxTextureSize() const;
        int getMaxDrawBuffers() const;
        int getMaxLabelLength() const;
        GLExtensions_t getExtensions() const;

        void glObjectLabel(GLenum const& target,
            GLuint const& object,
            GLint const& length,
            char const* const& label);

        UVCoordSign getNativeUVSign() const override;

        GPUType getGPUType() const override;
        GPUMemoryType getGPUMemoryType() const override;
        ClipSpaceHandedness getClipSpaceHandedness() const override;

        GraphicsBackend getBackendName() const override;

        ResourceHandle<ColorTexture> getPresentationRenderTarget() const override;

    protected:
        void initImpl(ff::IAssetBundle* const& assetBundle) override;

        void preRenderImpl() override;
        void postRenderImpl() override;

        void onBackBufferDimensionsUpdated(const glm::ivec2& dimensions) override;

        TextureImp* createColorTextureImp(TextureFlag_t const& flags,
            TextureUsage_t const& usage) override;
        TextureImp* createDepthTextureImp(TextureFlag_t const& flags,
            TextureUsage_t const& usage) override;
        std::shared_ptr<ISamplerState> createSamplerStateImpl(const SamplerFilter& minFilter,
            const SamplerFilter& magFilter,
            const SamplerAddressMode& sAddressMode,
            const SamplerAddressMode& tAddressMode,
            const SamplerMipFilter& mipFilter) override;

        std::shared_ptr<IVertexDescriptor> createVertexDescriptorImpl(const VertexDescriptorInfo& info) override;

        std::shared_ptr<IRenderState> createRenderStateImpl(std::shared_ptr<IShader> const& shader,
            std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
            std::vector<RenderColorLayerState> const& renderColorLayerStates,
            std::optional<DepthBufferFormat> const& depthBufferFormat) override;
        std::shared_ptr<IDepthStencilState> createDepthStencilStateImpl(const DepthCompareFunction& depthCompareFunction,
            const bool& depthWriteEnabled) override;

        std::shared_ptr<IRenderPass> beginRenderPassImpl(std::vector<std::pair<ResourceHandle<ColorTexture>,
                RenderTargetColorAttachmentOptions>> const& colorTargets,
            std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget) override;
        std::shared_ptr<IBlitPass> beginBlitPassImpl() override;

        std::shared_ptr<IShader> getShaderImpl(const std::string& shaderName) override;

        IndexBufferImp* createIndexBufferImp(IndexType const& type, const int& elementSize, const int& count) override;
        UniformBufferImp* createUniformBufferImp(const int& elementSize) override;
        VertexBufferImp* createVertexBufferImp(const int& elementSize, const int& count) override;

    private:
        char const* _vendorStr;
        char const* _rendererStr;
        char const* _versionStr;
        bool _isES;
        int _maxTextureUnits;
        int _maxTextureSize;
        int _maxDrawBuffers;
        int _maxLabelLength;
        GLExtensions_t _extensions;

        ResourceHandle<ColorTexture> _presentationRenderTarget;
        bool _hasRenderedToPresentationTarget;
        GLuint _readPFBO;

        ResourceHandle<GLShaderLibrary> _shaderLibrary;

        std::unique_ptr<GLGlobalState> _glState;
        std::unique_ptr<GLFramebufferManager> _fboManager;

        void initializeShaderLibrary(ff::IAssetBundle* const& assetBundle);
        void collectInfo();
    };
}

#endif
