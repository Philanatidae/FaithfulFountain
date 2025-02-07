/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/IGraphicsDevice.hpp>

#include <ff/graphics/NullTextureSource.hpp>
#include <ff/graphics/IdentityTextureSource.hpp>
#include <ff/graphics/DeferredBlitPass.hpp>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <ff/Locator.hpp>

namespace ff {
    IGraphicsDevice::IGraphicsDevice()
        :_nullSamplerState(nullptr),
        _identitySamplerState(nullptr),
        _backBufferDimensions(-1, -1) {
    }

    bool IGraphicsDevice::processEvent(const ResizeEvent& message) {
        setBackBufferDimensions(glm::ivec2(message.width, message.height));

        return false;
    }

    void IGraphicsDevice::init(ff::IAssetBundle* const& assetBundle) {
        FF_CONSOLE_LOG("Initializing graphics implementation...");
        initImpl(assetBundle);

        FF_CONSOLE_LOG("Reporting GPU info...");
        printGPUInfo();

        FF_CONSOLE_LOG("Initializing null resources...");
        initStaticResources();

        FF_CONSOLE_LOG("Registering for resize events...");
        Locator::getMessageBus().addListener<ResizeEvent>(this);

        FF_CONSOLE_LOG("Initialization complete.");
    }

    void IGraphicsDevice::preRender() {
        preRenderImpl();

        if(_currentPreRenderBlitPass) {
            auto blitPass = beginBlitPass();
            _currentPreRenderBlitPass->encode(blitPass.get());
            blitPass->end();
            _currentPreRenderBlitPass = nullptr;
        }
    }
    void IGraphicsDevice::postRender() {
        postRenderImpl();
    }

    void IGraphicsDevice::setBackBufferDimensions(const glm::ivec2& dimensions) {
        _backBufferDimensions = dimensions;
        FF_CONSOLE_LOG("Back buffer dimensions set: <%s, %s>",
           _backBufferDimensions.x,
           _backBufferDimensions.y);
        onBackBufferDimensionsUpdated(dimensions);
    }
    glm::ivec2 IGraphicsDevice::getBackBufferDimensions() const {
        return _backBufferDimensions;
    }

    glm::vec2 IGraphicsDevice::processUVCoords(const glm::vec2& coords) {
        return processUVCoords(coords.x, coords.y);
    }
    glm::vec2 IGraphicsDevice::processUVCoords(const float& u, const float& v) {
        if (getNativeUVSign() == UVCoordSign::POSITIVE_Y_UP) {
            // FF coordinates are negative Y up
            return glm::vec2(u, 1.0f - v);
        }

        return glm::vec2(u, v);
    }

    ResourceHandle<ColorTexture> IGraphicsDevice::getNullTexture() const {
        return Locator::getColorTextureManager().getTexture("null");
    }
    std::shared_ptr<ISamplerState> IGraphicsDevice::getNullSamplerState() const {
        return _nullSamplerState;
    }

    ResourceHandle<ColorTexture> IGraphicsDevice::getIdentityTexture() const {
        return Locator::getColorTextureManager().getTexture("identity");
    }
    std::shared_ptr<ISamplerState> IGraphicsDevice::getIdentitySamplerState() const {
        return _identitySamplerState;
    }

    ResourceHandle<Material> IGraphicsDevice::getNullMaterial() const {
        return _nullMaterial;
    }
    ResourceHandle<Material> IGraphicsDevice::getIdentityMaterial() const {
        return _identityMaterial;
    }

    std::shared_ptr<UniformBuffer<MeshUniforms>> IGraphicsDevice::getIdentityMeshUniforms() const {
        return _identityMeshUniforms;
    }

    template<>
    ResourceHandle<ColorTexture> IGraphicsDevice::createTexture(typename ColorTexture::bufferInfoType const& format,
        int const& width,
        int const& height,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label) {
        return ResourceHandle<ColorTexture>::createResource([this,
            format,
            width, height,
            flags,
            usage,
            label]() -> ColorTexture* {
            return createRawColorTexture(format,
                width, height,
                flags,
                usage,
                label);
        });
    }
    template<>
    ResourceHandle<DepthTexture> IGraphicsDevice::createTexture(typename DepthTexture::bufferInfoType const& format,
        int const& width,
        int const& height,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label) {
        return ResourceHandle<DepthTexture>::createResource([this,
            format,
            width, height,
            flags,
            usage,
            label]() -> DepthTexture* {
            return createRawDepthTexture(format,
                width, height,
                flags,
                usage,
                label);
        });
    }
    template<>
    ResourceHandle<ColorTexture> IGraphicsDevice::createTextureFromBackBufferDimensions(typename ColorTexture::bufferInfoType const& format,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        float const& hScale,
        float const& vScale,
        std::optional<std::string> const& label) {
        return ResourceHandle<ColorTexture>::createResource([this,
            format,
            hScale, vScale,
            flags,
            usage,
            label]() -> ColorTexture* {
            glm::ivec2 const bb = this->getBackBufferDimensions();
            return createRawColorTexture(format,
                bb.x * hScale, bb.y * vScale,
                flags,
                usage,
                label);
        });
    }
    template<>
    ResourceHandle<DepthTexture> IGraphicsDevice::createTextureFromBackBufferDimensions(typename DepthTexture::bufferInfoType const& format,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        float const& hScale,
        float const& vScale,
        std::optional<std::string> const& label) {
        return ResourceHandle<DepthTexture>::createResource([this,
            format,
            hScale, vScale,
            flags,
            usage,
            label]() -> DepthTexture* {
            glm::ivec2 const bb = this->getBackBufferDimensions();
            return createRawDepthTexture(format,
                bb.x * hScale, bb.y * vScale,
                flags,
                usage,
                label);
        });
    }
    std::shared_ptr<ISamplerState> IGraphicsDevice::createSamplerState(const SamplerFilter& minFilter,
        const SamplerFilter& magFilter,
        const SamplerAddressMode& sAddressMode,
        const SamplerAddressMode& tAddressMode,
        const SamplerMipFilter& mipFilter) {
        return createSamplerStateImpl(minFilter, magFilter,
            sAddressMode, tAddressMode,
            mipFilter);
    }

    std::shared_ptr<IDepthStencilState> IGraphicsDevice::createDepthStencilState(const DepthCompareFunction& depthCompareFunction,
        const bool& depthWriteEnabled) {
        return createDepthStencilStateImpl(depthCompareFunction, depthWriteEnabled);
    }
    std::shared_ptr<IVertexDescriptor> IGraphicsDevice::createVertexDescriptor(const VertexDescriptorInfo& info) {
        return createVertexDescriptorImpl(info);
    }

    std::shared_ptr<IRenderState> IGraphicsDevice::createRenderState(std::shared_ptr<IShader> const& shader,
        std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
        std::vector<RenderColorLayerState> const& renderColorLayerStates,
        std::optional<DepthBufferFormat> const& depthBufferFormat) {
        return createRenderStateImpl(shader,
            vertexDescriptor,
            renderColorLayerStates,
            depthBufferFormat);
    }
    
    std::shared_ptr<IRenderPass> IGraphicsDevice::beginRenderPass(std::vector<std::pair<ResourceHandle<ColorTexture>,
            RenderTargetColorAttachmentOptions>> const& colorTargets,
        std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget) {
        for(auto const& targets : colorTargets) {
            FF_ASSERT(targets.first != nullptr, "Color target cannot be null.");
        }
        if(depthTarget != std::nullopt) {
            FF_ASSERT(depthTarget->first != nullptr, "Depth target cannot be null");
        }
        return beginRenderPassImpl(colorTargets, depthTarget);
    }
    std::shared_ptr<IBlitPass> IGraphicsDevice::beginBlitPass() {
        return beginBlitPassImpl();
    }
    std::shared_ptr<IBlitPass> IGraphicsDevice::getPreRenderBlitPass() {
        if(!_currentPreRenderBlitPass) {
            _currentPreRenderBlitPass = std::make_shared<DeferredBlitPass>();
        }
        return _currentPreRenderBlitPass;
    }

    std::shared_ptr<IShader> IGraphicsDevice::getShader(const std::string& shaderName) {
        return getShaderImpl(shaderName);
    }

    ColorTexture* IGraphicsDevice::createRawColorTexture(ColorBufferFormat const& format,
        int const& width,
        int const& height,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label) {
        return new ColorTexture(createColorTextureImp(flags,
                usage),
            TextureType::COLOR,
            format,
            width,
            height,
            flags,
            usage,
            label);
    }
    DepthTexture* IGraphicsDevice::createRawDepthTexture(DepthBufferFormat const& format,
        int const& width,
        int const& height,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label) {
        return new DepthTexture(createDepthTextureImp(flags,
                usage),
            TextureType::DEPTH,
            format,
            width,
            height,
            flags,
            usage,
            label);
    }

    void IGraphicsDevice::initStaticResources() {
        ResourceHandle<TextureData> nullTextureData = ResourceHandle<TextureData>::createResource([]() -> TextureData* {
            auto nullTextureSource = getNullTextureSource();
            // @todo Is there a way that manual size can be removed? This was a recent fatal crash.
            return new TextureData(nullTextureSource.data(), nullTextureSource.size(), 32, 32, TextureFormat::RGBA8Unorm, false);
        });
        Locator::getColorTextureManager().addTexture("null", createTexture<ColorTexture>(convertTextureFormatToColorBufferFormat(nullTextureData->getFormat()),
            nullTextureData->getWidth(),
            nullTextureData->getHeight(),
            0,
            TextureUsage::GPU_SAMPLE
                | TextureUsage::GPU_BLIT
                | TextureUsage::CPU_WRITE,
            "NULL"));
        Locator::getColorTextureManager().getTexture("null")->bufferImage(nullTextureData);

        _nullSamplerState = createSamplerState(SamplerFilter::LINEAR, SamplerFilter::LINEAR,
            SamplerAddressMode::REPEAT_MIRROR, SamplerAddressMode::REPEAT_MIRROR);

        ResourceHandle<TextureData> identityTextureData = ResourceHandle<TextureData>::createResource([]() -> TextureData* {
            auto identityTextureSource = getIdentityTextureSource();
            // @todo Is there a way that manual size can be removed? This was a recent fatal crash.
            return new TextureData(identityTextureSource.data(), identityTextureSource.size(), 1, 1, TextureFormat::RGBA8Unorm, false);
        });
        Locator::getColorTextureManager().addTexture("identity", createTexture<ColorTexture>(convertTextureFormatToColorBufferFormat(identityTextureData->getFormat()),
            identityTextureData->getWidth(),
            identityTextureData->getHeight(),
            0,
            TextureUsage::GPU_SAMPLE
                | TextureUsage::GPU_BLIT
                | TextureUsage::CPU_WRITE,
            "IDENTITY"));
        // @todo Use blit instead of writing from the CPU
        Locator::getColorTextureManager().getTexture("identity")->bufferImage(identityTextureData);

        _identitySamplerState = createSamplerState(SamplerFilter::NEAREST, SamplerFilter::NEAREST,
            SamplerAddressMode::CLAMP_TO_EDGE, SamplerAddressMode::CLAMP_TO_EDGE);

        _nullMaterial = ResourceHandle<Material>::createResource([this]() -> Material* {
            return new Material("mesh", ff::Color::white(), getNullTexture(), getNullSamplerState());
        });
        _identityMaterial = ResourceHandle<Material>::createResource([this]() -> Material* {
            return new Material("mesh", ff::Color::white(), getIdentityTexture(), getIdentitySamplerState());
        });

        _identityMeshUniforms = createUniformBuffer<MeshUniforms>();
        MeshUniforms identityMeshUniforms = MeshUniforms(glm::identity<glm::mat4>());
        _identityMeshUniforms->bufferData(&identityMeshUniforms);
    }
    void IGraphicsDevice::printGPUInfo() {
        FF_CONSOLE_LOG("Native UV sign: %s", getNativeUVSign() == UVCoordSign::POSITIVE_Y_UP ? "Up = Positive Y" : "Up = Negative Y");

        FF_CONSOLE_LOG("GPU type: %s", gpuTypeToString(getGPUType()));
        FF_CONSOLE_LOG("GPU memory type: %s", gpuMemoryTypeToString(getGPUMemoryType()));
        FF_CONSOLE_LOG("Clip space handedness: %s", clipSpaceHandednessToString(getClipSpaceHandedness()));
    }
}
