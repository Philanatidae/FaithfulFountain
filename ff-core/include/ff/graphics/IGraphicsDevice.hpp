/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_I_GRAPHICS_DEVICE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_I_GRAPHICS_DEVICE_HPP

#include <glm/glm.hpp>
#include <optional>
#include <utility>

#include <ff/assets/IAssetBundle.hpp>

#include <ff/graphics/Texture.hpp>
#include <ff/graphics/GPUInfo.hpp>
#include <ff/graphics/ISamplerState.hpp>
#include <ff/graphics/VertexBuffer.hpp>
#include <ff/graphics/IndexBuffer.hpp>
#include <ff/graphics/UniformBuffer.hpp>
#include <ff/graphics/TextureData.hpp>
#include <ff/graphics/IDepthStencilState.hpp>
#include <ff/graphics/IRenderState.hpp>
#include <ff/graphics/IShader.hpp>
#include <ff/graphics/IVertexDescriptor.hpp>
#include <ff/graphics/IRenderPass.hpp>
#include <ff/graphics/IBlitPass.hpp>

#include <ff/graphics/DeferredBlitPass.hpp>

#include <ff/graphics/Material.hpp>
#include <ff/graphics/MeshUniforms.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff/events/ResizeEvent.hpp>

namespace ff {
    enum class GraphicsBackend {
        METAL,
        OPENGL,
        NULL_IMP
    };

    class IGraphicsDevice : public EventListener<ResizeEvent> {
    friend struct DefaultAssetLoader<ColorTexture>;

    public:
        IGraphicsDevice();
        virtual ~IGraphicsDevice() = default;

        bool processEvent(const ResizeEvent& message) override;

        void init(ff::IAssetBundle* const& assetBundle);

        void preRender();
        void postRender();

        void setBackBufferDimensions(const glm::ivec2& dimensions);
        glm::ivec2 getBackBufferDimensions() const;

        virtual UVCoordSign getNativeUVSign() const = 0;
        glm::vec2 processUVCoords(const glm::vec2& coords);
        glm::vec2 processUVCoords(const float& u, const float& v);

        virtual GPUType getGPUType() const = 0;
        virtual GPUMemoryType getGPUMemoryType() const = 0;
        virtual ClipSpaceHandedness getClipSpaceHandedness() const = 0;

        virtual GraphicsBackend getBackendName() const = 0;

        ResourceHandle<ColorTexture> getNullTexture() const;
        std::shared_ptr<ISamplerState> getNullSamplerState() const;

        ResourceHandle<ColorTexture> getIdentityTexture() const;
        std::shared_ptr<ISamplerState> getIdentitySamplerState() const;

        ResourceHandle<Material> getNullMaterial() const;
        ResourceHandle<Material> getIdentityMaterial() const;

        std::shared_ptr<UniformBuffer<MeshUniforms>> getIdentityMeshUniforms() const;

        virtual ResourceHandle<ColorTexture> getPresentationRenderTarget() const = 0;

        template<IndexType T = IndexType::UINT32>
        std::shared_ptr<IndexBuffer<T>> createIndexBuffer(const int& count);
        template<typename T>
        std::shared_ptr<UniformBuffer<T>> createUniformBuffer();
        template<typename T>
        std::shared_ptr<VertexBuffer<T>> createVertexBuffer(const int& count);

        template<typename T>
        ResourceHandle<T> createTexture(typename T::bufferInfoType const& format,
            int const& width,
            int const& height,
            TextureFlag_t const& flags,
            TextureUsage_t const& usage,
            std::optional<std::string> const& label = std::nullopt);
        template<typename T>
        ResourceHandle<T> createTextureFromBackBufferDimensions(typename T::bufferInfoType const& format,
            TextureFlag_t const& flags,
            TextureUsage_t const& usage,
            float const& hScale,
            float const& vScale,
            std::optional<std::string> const& label = std::nullopt);
        std::shared_ptr<ISamplerState> createSamplerState(const SamplerFilter& minFilter,
            const SamplerFilter& magFilter,
            const SamplerAddressMode& sAddressMode,
            const SamplerAddressMode& tAddressMode,
            const SamplerMipFilter& mipFilter = SamplerMipFilter::INVALID);

        std::shared_ptr<IDepthStencilState> createDepthStencilState(const DepthCompareFunction& depthCompareFunction,
            const bool& depthWriteEnabled = true);
        std::shared_ptr<IVertexDescriptor> createVertexDescriptor(const VertexDescriptorInfo& info);

        std::shared_ptr<IRenderState> createRenderState(std::shared_ptr<IShader> const& shader,
            std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
            std::vector<RenderColorLayerState> const& renderColorLayerStates,
            std::optional<DepthBufferFormat> const& depthBufferFormat = std::nullopt);
        
        std::shared_ptr<IRenderPass> beginRenderPass(std::vector<std::pair<ResourceHandle<ColorTexture>,
                RenderTargetColorAttachmentOptions>> const& colorTargets,
            std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget = std::nullopt);
        std::shared_ptr<IBlitPass> beginBlitPass();
        std::shared_ptr<IBlitPass> getPreRenderBlitPass();

        std::shared_ptr<IShader> getShader(const std::string& shaderName);

    protected:
        virtual void initImpl(ff::IAssetBundle* const& assetBundle) = 0;

        virtual void preRenderImpl() = 0;
        virtual void postRenderImpl() = 0;

        virtual void onBackBufferDimensionsUpdated(const glm::ivec2& dimensions) = 0;

        virtual TextureImp* createColorTextureImp(TextureFlag_t const& flags,
            TextureUsage_t const& usage) = 0;
        virtual TextureImp* createDepthTextureImp(TextureFlag_t const& flags,
            TextureUsage_t const& usage) = 0;
        virtual std::shared_ptr<ISamplerState> createSamplerStateImpl(const SamplerFilter& minFilter,
            const SamplerFilter& magFilter,
            const SamplerAddressMode& sAddressMode,
            const SamplerAddressMode& tAddressMode,
            const SamplerMipFilter& mipFilter) = 0;
        
        virtual std::shared_ptr<IDepthStencilState> createDepthStencilStateImpl(const DepthCompareFunction& depthCompareFunction,
            const bool& depthWriteEnabled) = 0;
        virtual std::shared_ptr<IVertexDescriptor> createVertexDescriptorImpl(const VertexDescriptorInfo& info) = 0;
        
        virtual std::shared_ptr<IRenderState> createRenderStateImpl(std::shared_ptr<IShader> const& shader,
            std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
            std::vector<RenderColorLayerState> const& renderColorLayerStates,
            std::optional<DepthBufferFormat> const& depthBufferFormat) = 0;
        
        virtual std::shared_ptr<IRenderPass> beginRenderPassImpl(std::vector<std::pair<ResourceHandle<ColorTexture>,
                RenderTargetColorAttachmentOptions>> const& colorTargets,
            std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget) = 0;
        virtual std::shared_ptr<IBlitPass> beginBlitPassImpl() = 0;

        virtual std::shared_ptr<IShader> getShaderImpl(const std::string& shaderName) = 0;

        virtual IndexBufferImp* createIndexBufferImp(IndexType const& type, const int& elementSize, const int& count) = 0;
        virtual UniformBufferImp* createUniformBufferImp(const int& elementSize) = 0;
        virtual VertexBufferImp* createVertexBufferImp(const int& elementSize, const int& count) = 0;

    private:
        glm::ivec2 _backBufferDimensions;
        std::shared_ptr<ISamplerState> _nullSamplerState;
        std::shared_ptr<ISamplerState> _identitySamplerState;
        ResourceHandle<Material> _nullMaterial;
        ResourceHandle<Material> _identityMaterial;
        std::shared_ptr<UniformBuffer<MeshUniforms>> _identityMeshUniforms;

        std::shared_ptr<DeferredBlitPass> _currentPreRenderBlitPass;

        ColorTexture* createRawColorTexture(ColorBufferFormat const& format,
            int const& width,
            int const& height,
            TextureFlag_t const& flags,
            TextureUsage_t const& usage,
            std::optional<std::string> const& label);
        DepthTexture* createRawDepthTexture(DepthBufferFormat const& format,
            int const& width,
            int const& height,
            TextureFlag_t const& flags,
            TextureUsage_t const& usage,
            std::optional<std::string> const& label);

        void initStaticResources();
        void printGPUInfo();
    };

    template<IndexType T>
    std::shared_ptr<IndexBuffer<T>> IGraphicsDevice::createIndexBuffer(const int& count) {
        return std::make_shared<IndexBuffer<T>>(createIndexBufferImp(T, sizeof(typename IndexTypeUnderlyingType<T>::type), count), count);
    }
    template<typename T>
    std::shared_ptr<UniformBuffer<T>> IGraphicsDevice::createUniformBuffer() {
        return std::make_shared<UniformBuffer<T>>(createUniformBufferImp(sizeof(T)));
    }
    template<typename T>
    std::shared_ptr<VertexBuffer<T>> IGraphicsDevice::createVertexBuffer(const int& count) {
        return std::make_shared<VertexBuffer<T>>(createVertexBufferImp(sizeof(T), count), count);
    }

    template<>
    ResourceHandle<ColorTexture> IGraphicsDevice::createTexture(typename ColorTexture::bufferInfoType const& format,
        int const& width,
        int const& height,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label);
    template<>
    ResourceHandle<DepthTexture> IGraphicsDevice::createTexture(typename DepthTexture::bufferInfoType const& format,
        int const& width,
        int const& height,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label);
    template<>
    ResourceHandle<ColorTexture> IGraphicsDevice::createTextureFromBackBufferDimensions(typename ColorTexture::bufferInfoType const& format,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        float const& hScale,
        float const& vScale,
        std::optional<std::string> const& label);
    template<>
    ResourceHandle<DepthTexture> IGraphicsDevice::createTextureFromBackBufferDimensions(typename DepthTexture::bufferInfoType const& format,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        float const& hScale,
        float const& vScale,
        std::optional<std::string> const& label);

    class NullGraphicsDevice : public IGraphicsDevice {
    public:
        virtual ~NullGraphicsDevice() = default;

        UVCoordSign getNativeUVSign() const override {
            return UVCoordSign::NEGATIVE_Y_UP;
        }

        GPUType getGPUType() const override {
            return GPUType::INTERMEDIATE;
        }
        GPUMemoryType getGPUMemoryType() const override {
            return GPUMemoryType::DISCRETE;
        }
        ClipSpaceHandedness getClipSpaceHandedness() const override {
            return ClipSpaceHandedness::LEFT;
        }

        GraphicsBackend getBackendName() const override {
            return GraphicsBackend::NULL_IMP;
        }

        ResourceHandle<ColorTexture> getPresentationRenderTarget() const override {
            return _presentationTarget;
        }

    protected:
        void initImpl(ff::IAssetBundle* const& assetBundle) override {
            _presentationTarget = createTexture<ColorTexture>(ColorBufferFormat::RGBA8Unorm,
                1, 1,
                0,
                TextureUsage::GPU_RENDER_TARGET | TextureUsage::GPU_STORE);
        }

        void preRenderImpl() override {
        }
        void postRenderImpl() override {
        }

        void onBackBufferDimensionsUpdated(const glm::ivec2& dimensions) override {
        }

        TextureImp* createColorTextureImp(TextureFlag_t const& flags,
            TextureUsage_t const& usage) override {
            return new NullTextureImp();
        }
        TextureImp* createDepthTextureImp(TextureFlag_t const& flags,
            TextureUsage_t const& usage) override {
            return new NullTextureImp();
        }
        std::shared_ptr<ISamplerState> createSamplerStateImpl(const SamplerFilter& minFilter,
            const SamplerFilter& magFilter,
            const SamplerAddressMode& sAddressMode,
            const SamplerAddressMode& tAddressMode,
            const SamplerMipFilter& mipFilter) override {
            return std::make_shared<NullSamplerState>(minFilter, magFilter,
                sAddressMode, tAddressMode,
                mipFilter);
        }
        
        std::shared_ptr<IDepthStencilState> createDepthStencilStateImpl(const DepthCompareFunction& depthCompareFunction,
            const bool& depthWriteEnabled) override {
            return std::make_shared<NullDepthStencilState>(depthCompareFunction, depthWriteEnabled);
        }
        std::shared_ptr<IVertexDescriptor> createVertexDescriptorImpl(const VertexDescriptorInfo& info) override {
            return std::make_shared<NullVertexDescriptor>(info);
        }
        
        std::shared_ptr<IRenderState> createRenderStateImpl(std::shared_ptr<IShader> const& shader,
            std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
            std::vector<RenderColorLayerState> const& renderColorLayerStates,
            std::optional<DepthBufferFormat> const& depthBufferFormat) override {
            return std::make_shared<NullRenderState>(shader,
                vertexDescriptor,
                renderColorLayerStates,
                depthBufferFormat);
        }
        
        std::shared_ptr<IRenderPass> beginRenderPassImpl(std::vector<std::pair<ResourceHandle<ColorTexture>,
                RenderTargetColorAttachmentOptions>> const& colorTargets,
            std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget) override {
            return std::make_shared<NullRenderPass>(colorTargets, depthTarget);
        }
        std::shared_ptr<IBlitPass> beginBlitPassImpl() override {
            return std::make_shared<NullBlitPass>();
        }

        std::shared_ptr<IShader> getShaderImpl(const std::string& shaderName) override {
            return std::make_shared<NullShader>();
        }

        IndexBufferImp* createIndexBufferImp(IndexType const& type, const int& elementSize, const int& count) override {
            return new NullIndexBufferImp();
        }
        UniformBufferImp* createUniformBufferImp(const int& elementSize) override {
            return new NullUniformBufferImp();
        }
        VertexBufferImp* createVertexBufferImp(const int& elementSize, const int& count) override {
            return new NullVertexBufferImp();
        }

    private:
        ResourceHandle<ColorTexture> _presentationTarget;
    };
}

#endif
