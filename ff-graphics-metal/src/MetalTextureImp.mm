/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-metal/MetalTextureImp.hmm>

#include <ff-graphics-metal/MetalBufferFormats.hmm>

namespace ff {

MetalTextureImp::MetalTextureImp(MetalGraphicsDevice* const& graphicsDevice)
    :_graphicsDevice(graphicsDevice),
    _nonOwning(false) {
}
MetalTextureImp::MetalTextureImp(MetalGraphicsDevice* const& graphicsDevice,
    id<MTLTexture> nonOwnedTexture)
    :_graphicsDevice(graphicsDevice),
    _nonOwning(true),
    _texture(nonOwnedTexture) {
}

id<MTLTexture> MetalTextureImp::getMTLTexture() const {
    return _texture;
}

void MetalTextureImp::init(TextureType const& type,
    int const& width,
    int const& height,
    ColorBufferFormat const& format,
    TextureFlag_t const& flags,
    TextureUsage_t const& usage,
    std::optional<std::string> const& label) {
    FF_ASSERT(type == TextureType::COLOR, "Render target type must be COLOR to use ColorBufferFormat.");

    if(_nonOwning) {
        FF_ASSERT((usage & (TextureUsage::GPU_SAMPLE
            | TextureUsage::CPU_READ
            | TextureUsage::CPU_WRITE)) == 0,
            "Non-owning render target cannot be sampled/have CPU access.");
        FF_ASSERT(width == _texture.width, "Width does not match non-owned texture width.");
        FF_ASSERT(height == _texture.height, "Height does not match non-owned texture height.");

        return;
    }

    createTexture(width,
        height,
        convertBufferFormatToMetalPixelFormat(format),
        flags,
        usage,
        label);
}
void MetalTextureImp::init(TextureType const& type,
    int const& width,
    int const& height,
    DepthBufferFormat const& format,
    TextureFlag_t const& flags,
    TextureUsage_t const& usage,
    std::optional<std::string> const& label) {
    FF_ASSERT(type == TextureType::DEPTH, "Render target type must be DEPTH to use DepthBufferFormat.");

    if(_nonOwning) {
        FF_ASSERT((usage & (TextureUsage::GPU_SAMPLE
            | TextureUsage::CPU_READ
            | TextureUsage::CPU_WRITE)) == 0,
            "Non-owning render target cannot be sampled/have CPU access.");
        FF_ASSERT(width == _texture.width, "Width does not match non-owned texture width.");
        FF_ASSERT(height == _texture.height, "Height does not match non-owned texture height.");

        return;
    }

    createTexture(width,
        height,
        convertBufferFormatToMetalPixelFormat(format),
        flags,
        usage,
        label);
}

void MetalTextureImp::readPixels(void* dataOut,
    BufferFormatInfo_t const& formatInfo,
    int const& x,
    int const& y,
    int const& width,
    int const& height) {
    [_texture
        getBytes:dataOut
        bytesPerRow:formatInfo.size * width * height
        fromRegion:MTLRegionMake2D((NSUInteger)x,
            (NSUInteger)y,
            (NSUInteger)width,
            (NSUInteger)height)
        mipmapLevel:0];
}

void MetalTextureImp::bufferImage(void* const& data,
    BufferFormatInfo_t const& formatInfo,
    int const& mipLevel,
    int const& subWidth,
    int const& subHeight,
    int const& xoffset,
    int const& yoffset) {
    std::string str;
    [_texture replaceRegion:MTLRegionMake2D(xoffset,
        yoffset,
        subWidth,
        subHeight)
        mipmapLevel:mipLevel
        withBytes:data
        bytesPerRow:(NSUInteger)(formatInfo.size * subWidth)];
}

void MetalTextureImp::createTexture(int const& width,
    int const& height,
    MTLPixelFormat const& pixelFormat,
    TextureFlag_t const& flags,
    TextureUsage_t const& usage,
    std::optional<std::string> const& label) {
    FF_ASSERT(!_nonOwning, "Attempted to overwrite non-owning textured.");

    MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
        width:width
        height:height
        mipmapped:NO];

    bool const gpuRenderTarget = (usage & TextureUsage::GPU_RENDER_TARGET) > 0;
    bool const gpuStore = (usage & TextureUsage::GPU_STORE) > 0;
    bool const gpuLoad = (usage & TextureUsage::GPU_LOAD) > 0;
    bool const gpuSample = (usage & TextureUsage::GPU_SAMPLE) > 0;
    bool const gpuBlit = (usage & TextureUsage::GPU_BLIT) > 0;
    bool const cpuRead = (usage & TextureUsage::CPU_READ) > 0;
    bool const cpuWrite = (usage & TextureUsage::CPU_WRITE) > 0;

    desc.allowGPUOptimizedContents = (cpuRead || cpuWrite) ? NO : YES;
    if(cpuWrite && !cpuRead) {
        desc.cpuCacheMode = MTLCPUCacheModeWriteCombined;
    }

    // @todo Allow for managed memory (more efficient on discrete GPUs).
    desc.storageMode = (cpuRead ||cpuWrite) ? MTLStorageModeShared : MTLStorageModePrivate;
    if(@available(iOS 11, macOS 11, tvOS 10, *)) {
        if(desc.storageMode == MTLStorageModePrivate
            // Memoryless is meant for render passes,
            // but it is not a strict requirement.
            && !gpuStore
            && !gpuLoad
            && !gpuSample
            && !gpuBlit) {
            desc.storageMode = MTLStorageModeMemoryless;
        }
    }

    desc.usage = 0;
    if(gpuRenderTarget) {
        desc.usage |= MTLTextureUsageRenderTarget;
    }
    if(gpuSample) {
        desc.usage |= MTLTextureUsageShaderRead;
    }

    _texture = [_graphicsDevice->getDevice() newTextureWithDescriptor:desc];
    if(label) {
        _texture.label = [NSString stringWithUTF8String:label->c_str()];
    }
}

void MetalTextureImp::bindToRenderPassDescriptorAsColor(MTLRenderPassDescriptor* renderPassDescriptor,
    ColorBufferFormat const& format,
    RenderTargetColorAttachmentOptions const& options,
    int const& slot) {
    MTLRenderPassColorAttachmentDescriptor* attachment = renderPassDescriptor.colorAttachments[slot];
    attachment.texture = _texture;

    // @todo Support MSAA
    switch(options.loadAction) {
    case RenderTargetLoadAction::CLEAR:
        attachment.loadAction = MTLLoadActionClear;
        attachment.clearColor = MTLClearColorMake(options.clearColor.r,
            options.clearColor.g,
            options.clearColor.b,
            options.clearColor.a);
        break;
    case RenderTargetLoadAction::LOAD:
        attachment.loadAction = MTLLoadActionLoad;
        break;
    case RenderTargetLoadAction::DONT_CARE:
        attachment.loadAction = MTLLoadActionDontCare;
        break;
    default:
        FF_CONSOLE_ERROR("Unsupported load action: %s", (int)options.loadAction);
    }
    switch(options.storeAction) {
    case RenderTargetStoreAction::STORE:
        attachment.storeAction = MTLStoreActionStore;
        break;
    case RenderTargetStoreAction::DONT_CARE:
        attachment.storeAction = MTLStoreActionDontCare;
        break;
    default:
        FF_CONSOLE_ERROR("Unsupported store action: %s", (int)options.storeAction);
    }
}
void MetalTextureImp::bindToRenderPassDescriptorAsDepth(MTLRenderPassDescriptor* renderPassDescriptor,
    DepthBufferFormat const& format,
    RenderTargetDepthAttachmentOptions const& options) {
    MTLRenderPassDepthAttachmentDescriptor* attachment = renderPassDescriptor.depthAttachment;
    attachment.texture = _texture;

    // @todo Support MSAA
    switch(options.loadAction) {
    case RenderTargetLoadAction::CLEAR:
        attachment.loadAction = MTLLoadActionClear;
        attachment.clearDepth = options.clearDepth;
        break;
    case RenderTargetLoadAction::LOAD:
        attachment.loadAction = MTLLoadActionLoad;
        break;
    case RenderTargetLoadAction::DONT_CARE:
        attachment.loadAction = MTLLoadActionDontCare;
        break;
    default:
        FF_CONSOLE_ERROR("Unsupported load action: %s", (int)options.loadAction);
    }
    switch(options.storeAction) {
    case RenderTargetStoreAction::STORE:
        attachment.storeAction = MTLStoreActionStore;
        break;
    case RenderTargetStoreAction::DONT_CARE:
        attachment.storeAction = MTLStoreActionDontCare;
        break;
    default:
        FF_CONSOLE_ERROR("Unsupported store action: %s", (int)options.storeAction);
    }
}

}

