/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-metal/MetalGraphicsDevice.hmm>

#include <ff/CVars.hpp>
#include <ff/Locator.hpp>

#include <ff/graphics/FullscreenRenderTarget.hpp>
#include <ff/graphics/PPQuad.hpp>
#include <ff/graphics/VertexPositionTextureColor.hpp>

#include <memory>

#include <ff/graphics/IndexBuffer.hpp>
#include <ff/graphics/VertexBuffer.hpp>
#include <ff/graphics/UniformBuffer.hpp>

#include <ff-graphics-metal/MetalBlitPass.hmm>
#include <ff-graphics-metal/MetalBuffer.hmm>
#include <ff-graphics-metal/MetalDepthStencilState.hmm>
#include <ff-graphics-metal/MetalRenderState.hmm>
#include <ff-graphics-metal/MetalRenderPass.hmm>
#include <ff-graphics-metal/MetalSamplerState.hmm>
#include <ff-graphics-metal/MetalShaderLibrary.hmm>
#include <ff-graphics-metal/MetalTextureImp.hmm>
#include <ff-graphics-metal/MetalVertexDescriptor.hmm>

namespace ff {
    MetalGraphicsDevice::MetalGraphicsDevice()
        :_layer(nil),
        _didRenderPassOfPresentationRenderTarget(false),
        _imgui(false) {
    }

    id<MTLDevice> MetalGraphicsDevice::getDevice() const {
        return _device;
    }
    CAMetalLayer* MetalGraphicsDevice::getLayer() const {
        return _layer;
    }
    id<MTLCommandBuffer> MetalGraphicsDevice::getCurrentCommandBuffer() {
        return _currentCommandBuffer;
    }

    UVCoordSign MetalGraphicsDevice::getNativeUVSign() const {
        return UVCoordSign::NEGATIVE_Y_UP;
    }

    GPUType MetalGraphicsDevice::getGPUType() const {
        if(_device == nil) {
            return GPUType::UNKNOWN;
        }

        if([_device.name rangeOfString:@"Apple"].location != NSNotFound) {
            // All Apple GPU's are tile based
            return GPUType::TILE;
        }
        // All others Apple uses (Intel, AMD) are intermediate
        return GPUType::INTERMEDIATE;
    }
    GPUMemoryType MetalGraphicsDevice::getGPUMemoryType() const {
        if(_device == nil) {
            return GPUMemoryType::UNKNOWN;
        }
        
        if([_device.name rangeOfString:@"Apple"].location != NSNotFound) {
            // There are currently no (and likely won't be) Apple GPU's that have dedicated memory
            return GPUMemoryType::SHARED;
        }
        return GPUMemoryType::DISCRETE;
    }
    ClipSpaceHandedness MetalGraphicsDevice::getClipSpaceHandedness() const {
        return ClipSpaceHandedness::LEFT;
    }

    GraphicsBackend MetalGraphicsDevice::getBackendName() const {
        return GraphicsBackend::METAL;
    }

    ResourceHandle<ColorTexture> MetalGraphicsDevice::getPresentationRenderTarget() const {
        return _presentationRenderTarget->getColorRenderTarget();
    }

    void MetalGraphicsDevice::initImpl(ff::IAssetBundle* const& assetBundle) {
        initializeDevice();
        initializeShaderLibrary(assetBundle);
        initializeLayer();

        _commandQueue = [_device newCommandQueue];
    }
    void MetalGraphicsDevice::preRenderImpl() {
       _currentDrawable = [_layer nextDrawable];
       FF_ASSERT(_currentDrawable, "Could not obtain next drawable."); // @todo Allow not obtaining next drawable as valid (skip rendering for this frame?)
        
        Locator::getStatistics().beginStopwatch("game render");

        _didRenderPassOfPresentationRenderTarget = false;
        
        // Update VSync
        // @todo vvv
//       _layer.displaySyncEnabled = ff::CVars::get<bool>("graphics_vsync");

        // Create buffer
        _currentCommandBuffer = [_commandQueue commandBuffer];
    }
    void MetalGraphicsDevice::postRenderImpl() {
        if(!_didRenderPassOfPresentationRenderTarget) {
            // We need a dummy render pass to clear the presentation render target.
            // Normally the game will do this (because the game is always rendering
            // to the presentation render target), this is likely being caused
            // by the game not yet being completely initialized.
            beginRenderPass({
                std::make_pair(getPresentationRenderTarget(),
                    _presentationDummyAttachOptions)
            })->end();
        }

        // Render game's presentation render target to the "final" (screen) render target.
        // Game renders using RGBA, Metal must be BGRA. We can't do this any other way
        // besides using a render pass.
        // @todo Automate this by having allowing the game to use a platform default color format.
        // @todo May look into pixel views for this instead, might give the best
        //  of both worlds.
        // @todo May convert back into pure Metal render pass.
        ResourceHandle<ColorTexture> finalPresentationRenderTarget = ResourceHandle<ColorTexture>::createResource([this]() -> ColorTexture* {
            return new ColorTexture(new MetalTextureImp(this, _currentDrawable.texture),
                TextureType::COLOR,
                ColorBufferFormat::Native,
                _currentDrawable.texture.width,
                _currentDrawable.texture.height,
                0,
                TextureUsage::GPU_STORE
                    | TextureUsage::GPU_RENDER_TARGET
                    | TextureUsage::GPU_BLIT,
                std::nullopt);
        });
        {
            RenderTargetColorAttachmentOptions options;
            options.loadAction = RenderTargetLoadAction::DONT_CARE;
            options.storeAction = RenderTargetStoreAction::STORE;
            auto finalPresentationRenderPass = beginRenderPass({
                std::make_pair(finalPresentationRenderTarget,
                    options)
            });
            finalPresentationRenderPass->setTexture(getPresentationRenderTarget(), 0);
            finalPresentationRenderPass->setSamplerState(_finalPresentationSamplerState, 0);
            _finalPresentationPPQuad->draw(*finalPresentationRenderPass);
            finalPresentationRenderPass->end();
        }

        // @todo On macOS 10.15.4 and later, we should be able to
        // grab the drawable's presentation time and use that for
        // delta smoothing. For some reason, this is currently
        // returning 0.0 all the time, and I'm not sure why. This
        // is available on iOS and tvOS, and should be used, so
        // this should be tested on iOS to see if this is correct or not.
        // if ([_currentDrawable respondsToSelector: @selector(addPresentedHandler:)]) {
        //     // addPresentedHandler and removePresentedHandler are only available on macOS 10.15.4 and later.
        //     [_currentDrawable addPresentedHandler:^(id<MTLDrawable> drawable) {
        //         // NSLog(@"%f", drawable.presentedTime);
        //     }];
        // }
        [_currentCommandBuffer presentDrawable:_currentDrawable];
        [_currentCommandBuffer commit];
        // Release drawable ASAP
        _currentDrawable = nil;
        // @todo Triple buffering
        [_currentCommandBuffer waitUntilCompleted];
        
        Locator::getStatistics().endStopwatch("game render");

        _currentCommandBuffer = nil;
        _currentCommandEncoder = nil;
    }

    void MetalGraphicsDevice::onBackBufferDimensionsUpdated(const glm::ivec2& dimensions) {
        if(_layer != nil) {
            _layer.drawableSize = CGSizeMake(getBackBufferDimensions().x,
                getBackBufferDimensions().y);
        }

        if(_presentationRenderTarget == nullptr) {
            // @todo This is in a bad spot, move to initialize. Problem is we need
            // back buffer dimensions to be correct before init on Metal,
            // and set after init on GL.
            initializePresentationRenderTarget();
        }
    }

    TextureImp* MetalGraphicsDevice::createColorTextureImp(TextureFlag_t const& flags,
        TextureUsage_t const& usage) {
        return new MetalTextureImp(this);
    }
    TextureImp* MetalGraphicsDevice::createDepthTextureImp(TextureFlag_t const& flags,
        TextureUsage_t const& usage) {
        return new MetalTextureImp(this);
    }
    std::shared_ptr<ISamplerState> MetalGraphicsDevice::createSamplerStateImpl(const SamplerFilter& minFilter,
            const SamplerFilter& magFilter,
            const SamplerAddressMode& sAddressMode,
            const SamplerAddressMode& tAddressMode,
            const SamplerMipFilter& mipFilter) {
        return std::make_shared<MetalSamplerState>(this, minFilter, magFilter, sAddressMode, tAddressMode, mipFilter);
    }

    std::shared_ptr<IVertexDescriptor> MetalGraphicsDevice::createVertexDescriptorImpl(const VertexDescriptorInfo& info) {
        return std::make_shared<MetalVertexDescriptor>(this, info);
    }
    std::shared_ptr<IDepthStencilState> MetalGraphicsDevice::createDepthStencilStateImpl(const DepthCompareFunction& depthCompareFunction,
        const bool& depthWriteEnabled) {
        return std::make_shared<MetalDepthStencilState>(*this, depthCompareFunction, depthWriteEnabled);
    }

    std::shared_ptr<IRenderState> MetalGraphicsDevice::createRenderStateImpl(std::shared_ptr<IShader> const& shader,
        std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
        std::vector<RenderColorLayerState> const& renderColorLayerStates,
        std::optional<DepthBufferFormat> const& depthBufferFormat) {
        return std::make_shared<MetalRenderState>(this,
            shader,
            vertexDescriptor,
            renderColorLayerStates,
            depthBufferFormat);
    }


    std::shared_ptr<IRenderPass> MetalGraphicsDevice::beginRenderPassImpl(std::vector<std::pair<ResourceHandle<ColorTexture>,
            RenderTargetColorAttachmentOptions>> const& colorTargets,
        std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget) {
        for(auto& colorTarget : colorTargets) {
            if(colorTarget.first == _presentationRenderTarget->getColorRenderTarget()) {
                _didRenderPassOfPresentationRenderTarget = true;
            }
        }

        return std::make_shared<MetalRenderPass>(this,
            colorTargets,
            depthTarget);
    }
    std::shared_ptr<IBlitPass> MetalGraphicsDevice::beginBlitPassImpl() {
        return std::make_shared<MetalBlitPass>(this);
    }

    std::shared_ptr<IShader> MetalGraphicsDevice::getShaderImpl(const std::string& shaderName) {
        return _shaderLibrary->getShader(shaderName);
    }

    IndexBufferImp* MetalGraphicsDevice::createIndexBufferImp(IndexType const& type, const int& elementSize, const int& count) {
        return new MetalBufferImp<IndexBufferImp>(this, elementSize * count);
    }
    UniformBufferImp* MetalGraphicsDevice::createUniformBufferImp(const int& elementSize) {
        return new MetalBufferImp<UniformBufferImp>(this, elementSize);
    }
    VertexBufferImp* MetalGraphicsDevice::createVertexBufferImp(const int& elementSize, const int& count) {
        return new MetalBufferImp<VertexBufferImp>(this, elementSize * count);
    }

    void MetalGraphicsDevice::initializeDevice() {
#if TARGET_OS_OSX
        // Metal device discovery (only available on macOS)
        NSArray<id<MTLDevice>>* devices = MTLCopyAllDevices();
        FF_CONSOLE_LOG("Available Metal devices:");
        for(id<MTLDevice> device in devices) {
            FF_CONSOLE_LOG("\t- %s", [[device name] UTF8String]);
        }
#endif
       _device = MTLCreateSystemDefaultDevice();
       FF_CONSOLE_LOG("Choosing device: %s", [[_device name] UTF8String]);
#if TARGET_OS_OSX
        // @todo Add ability to force specific device for macOS
#endif
    }
    void MetalGraphicsDevice::initializeShaderLibrary(ff::IAssetBundle* const& assetBundle) {
        _shaderLibrary = assetBundle->load<MetalShaderLibrary>("shader_library_main_metal");
    }
    void MetalGraphicsDevice::initializeLayer() {
        // This is only the initialization of the CAMetalLayer.
        // This does not attach the layer to the view; this
        // is handled differently in macOS and iOS.
        // After device creation and initialization, the platform
        // needs to take care of assigning the layer to a view.

       _layer = [CAMetalLayer layer];
       _layer.device = _device;
       _layer.pixelFormat = MTLPixelFormatBGRA8Unorm; // Cannot be RGBA8: https://developer.apple.com/documentation/quartzcore/cametallayer/1478155-pixelformat?language=objc
        // This isn't mentioned in the docs but it looks like you can't enable/disable
        // v-sync on iOS. (Not true, you can't when using CoreAnimation).
#if TARGET_OS_OSX
       _layer.displaySyncEnabled = CVars::get<bool>("graphics_vsync"); // Not truly necessary since VSync is updated any frame anyway
#endif
       _layer.allowsNextDrawableTimeout = NO; // Force Metal to wait for the next drawable if it isn't available
       
       _layer.drawableSize = CGSizeMake(getBackBufferDimensions().x, getBackBufferDimensions().y);
    }

    void MetalGraphicsDevice::initializePresentationRenderTarget() {
        FF_CONSOLE_LOG("Initializing presentation render target...");
        _presentationRenderTarget = std::make_shared<FullscreenRenderTarget>(ColorBufferFormat::RGBA8Unorm,
            TextureUsage::GPU_RENDER_TARGET
                | TextureUsage::GPU_STORE
                | TextureUsage::GPU_LOAD
                | TextureUsage::GPU_BLIT
                | TextureUsage::GPU_SAMPLE,
            "mtl_presentation_render_target");
        _presentationDummyAttachOptions.clearColor = ff::ClearColor::black();
        _presentationDummyAttachOptions.loadAction = RenderTargetLoadAction::CLEAR;
        _presentationDummyAttachOptions.storeAction = RenderTargetStoreAction::STORE;
        _finalPresentationSamplerState = createSamplerState(SamplerFilter::LINEAR,
            SamplerFilter::LINEAR,
            SamplerAddressMode::CLAMP_TO_EDGE,
            SamplerAddressMode::CLAMP_TO_EDGE);
        _finalPresentationPPQuad = std::make_shared<PPQuad>(getShader("sprite_no_camera"), ColorBufferFormat::RGBA8Unorm);

        // @todo Temporary, this needs to be reworked. Probably grab vertex descriptor (or
        // at least generate it) from the shader during processing. Render state needs to
        // be reworked as well. Probably won't happen until render graph is implemented.
        VertexDescriptorInfo vertexDescriptorInfo({
            BufferDescriptorInfo(0, sizeof(VertexPositionTextureColor), {
                AttributeDescriptorInfo(0, FF_OFFSET_OF(VertexPositionTextureColor, position), AttributeFormat::Float4), // Position
                AttributeDescriptorInfo(1, FF_OFFSET_OF(VertexPositionTextureColor, texCoord), AttributeFormat::Float2), // Tex Coord
                AttributeDescriptorInfo(2, FF_OFFSET_OF(VertexPositionTextureColor, color), AttributeFormat::Float4), // Color
            })
        });
        auto vertexDescriptor = createVertexDescriptor(vertexDescriptorInfo);

        auto finalPresentationRenderState = std::make_shared<MetalRenderState>(this,
            std::static_pointer_cast<MetalShader>(getShader("sprite_no_camera")),
            std::static_pointer_cast<MetalVertexDescriptor>(vertexDescriptor),
            std::vector<RenderColorLayerState>({
                RenderColorLayerState(ColorBufferFormat::Native)
            }),
            std::nullopt,
            std::vector<MTLPixelFormat>({
                MTLPixelFormatBGRA8Unorm
            }));
        _finalPresentationPPQuad->setRenderState(finalPresentationRenderState);
    }
}
