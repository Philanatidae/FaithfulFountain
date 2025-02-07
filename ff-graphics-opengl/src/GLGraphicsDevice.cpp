/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLGraphicsDevice.hpp>

#include <ff-graphics-opengl/GLGlobalState.hpp>
#include <ff-graphics-opengl/GLFramebufferManager.hpp>

#include <ff-graphics-opengl/GLBlitPass.hpp>
#include <ff-graphics-opengl/GLTextureImp.hpp>
#include <ff-graphics-opengl/GLShader.hpp>
#include <ff-graphics-opengl/GLShaderLibrary.hpp>
#include <ff-graphics-opengl/GLVertexBufferImp.hpp>
#include <ff-graphics-opengl/GLIndexBufferImp.hpp>
#include <ff-graphics-opengl/GLUniformBufferImp.hpp>
#include <ff-graphics-opengl/GLRenderPass.hpp>
#include <ff-graphics-opengl/GLDepthStencilState.hpp>
#include <ff-graphics-opengl/GLVertexDescriptor.hpp>
#include <ff-graphics-opengl/GLRenderState.hpp>
#include <ff-graphics-opengl/GLSamplerState.hpp>

#include <ff/CVars.hpp>
#include <ff/Locator.hpp>
#include <ff/graphics/Color.hpp>

#include <cstring>

namespace ff {
    GLGraphicsDevice::GLGraphicsDevice(const GLenum& presentationColorBufferFormat,
        const GLenum& presentationDepthBufferFormat)
        :_vendorStr(nullptr),
        _rendererStr(nullptr),
        _versionStr(nullptr),
        _maxTextureSize(-1),
        _maxTextureUnits(-1),
        _maxDrawBuffers(-1),
        _extensions(0),
        _isES(false),
        _presentationRenderTarget(ResourceHandle<ColorTexture>::createNullResource()),
        _hasRenderedToPresentationTarget(false),
        _readPFBO(0) {

        Locator::getMessageBus().addListener<ResizeEvent>(this);
    }
    GLGraphicsDevice::~GLGraphicsDevice() {
        if (_readPFBO != 0) {
            FF_GL_CALL(glDeleteFramebuffers, 1, &_readPFBO);
            _readPFBO = 0;
        }
    }

    GLGlobalState* GLGraphicsDevice::getGlobalState() {
        return _glState.get();
    }
    GLFramebufferManager* GLGraphicsDevice::getFramebufferManager() {
        return _fboManager.get();
    }

    char const* GLGraphicsDevice::getGLVendor() const {
        return _vendorStr;
    }
    char const* GLGraphicsDevice::getGLRenderer() const {
        return _rendererStr;
    }
    char const* GLGraphicsDevice::getGLVersion() const {
        return _versionStr;
    }
    bool GLGraphicsDevice::isGLES() const {
        return _isES;
    }
    int GLGraphicsDevice::getMaxTextureUnits() const {
        return _maxTextureUnits;
    }
    int GLGraphicsDevice::getMaxTextureSize() const {
        return _maxTextureSize;
    }
    int GLGraphicsDevice::getMaxDrawBuffers() const {
        return _maxDrawBuffers;
    }
    int GLGraphicsDevice::getMaxLabelLength() const {
        return _maxLabelLength;
    }
    GLExtensions_t GLGraphicsDevice::getExtensions() const {
        return _extensions;
    }

    void GLGraphicsDevice::glObjectLabel(GLenum const& target,
        GLuint const& object,
        GLint const& length,
        char const* const& label) {
        if ((getExtensions() & (GLExtensions::ARB_DEBUG_LABEL
            | GLExtensions::KHR_DEBUG)) == 0) {
            return;
        }

        if ((getExtensions() & GLExtensions::ARB_DEBUG_LABEL) > 0) {
            FF_GL_CALL(glObjectLabel,
                       target,
                       object,
                       glm::min<GLint>(length, getMaxLabelLength()),
                       label);
            return;
        }
#if defined(FF_GL_HAS_GL_OBJECT_LABEL_KHR)
        if ((getExtensions() & GLExtensions::KHR_DEBUG) > 0) {
            FF_GL_CALL(glObjectLabelKHR,
                target,
                object,
                glm::min<GLint>(length, getMaxLabelLength()),
                label);
        }
#endif
    }

    UVCoordSign GLGraphicsDevice::getNativeUVSign() const {
        return UVCoordSign::POSITIVE_Y_UP;
    }

    GPUType GLGraphicsDevice::getGPUType() const {
        if(isGLES()) {
            // If it's GLES, it's highly likely it is a tile renderer.
            // On Linux, it can be running under GLES, but we will not
            // optimize for GLES on desktop when they can just run
            // desktop GL.
            return GPUType::TILE;
        }
        // If the vendor is Apple, it is a tile GPU.
        if(strcmp(getGLVendor(), "Apple") == 0) {
            return GPUType::TILE;
        }
        // Everyone else is an intermediate GPU.
        return GPUType::INTERMEDIATE;
    }
    GPUMemoryType GLGraphicsDevice::getGPUMemoryType() const {
        if(isGLES()) {
            // If it's GLES, it's highly likely it is shared memory.
            // On Linux, it can be running under GLES, but we will not
            // supports builds with GLES on desktop.
            return GPUMemoryType::SHARED;
        }
        // If the vendor is Apple, it is shared memory.
        if(strcmp(getGLVendor(), "Apple") == 0) {
            return GPUMemoryType::SHARED;
        }
        // Everyone else is discrete memory.
        return GPUMemoryType::DISCRETE;
    }
    ClipSpaceHandedness GLGraphicsDevice::getClipSpaceHandedness() const {
        return ClipSpaceHandedness::LEFT;
    }

    GraphicsBackend GLGraphicsDevice::getBackendName() const {
        return GraphicsBackend::OPENGL;
    }

    ResourceHandle<ColorTexture> GLGraphicsDevice::getPresentationRenderTarget() const {
        // @todo Return an interface, blit to the final target. This allows
        // for double-buffering, and matches the behavior of other backends.
        // Hopefully performance isn't bad enough that we'd have to rethink this.
        return _presentationRenderTarget;
    }

    void GLGraphicsDevice::initImpl(ff::IAssetBundle* const& assetBundle) {
        collectInfo();
        _glState = std::make_unique<GLGlobalState>(this);
        _glState->init();
        _fboManager = std::make_unique<GLFramebufferManager>(this);
        _fboManager->init();
        initializeShaderLibrary(assetBundle);
    }

    void GLGraphicsDevice::preRenderImpl() {
        if(CVars::get<bool>("graphics_gl_print_api_calls")) {
            FF_CONSOLE_LOG("Begin frame render.");
        }

        ff::Locator::getStatistics().beginStopwatch("game render");
        _hasRenderedToPresentationTarget = false;
    }
    void GLGraphicsDevice::postRenderImpl() {
        if(CVars::get<bool>("graphics_gl_print_api_calls")) {
            FF_CONSOLE_LOG("End frame render.");
        }

        if (!_hasRenderedToPresentationTarget) {
            RenderTargetColorAttachmentOptions options;
            options.loadAction = RenderTargetLoadAction::DONT_CARE;
            options.storeAction = RenderTargetStoreAction::STORE;
            options.clearColor = ff::ClearColor::black();
            beginRenderPass({ std::make_pair(getPresentationRenderTarget(), options) })->end();
        }

        // We can't perform a standard FF blit pass because
        // we don't have a way to attach the final presentation
        // render target to the draw framebuffer. We have to do
        // the blit operations manually.
        getGlobalState()->bindReadFramebuffer(_readPFBO);
        getGlobalState()->bindDrawFramebuffer(0);
        getGlobalState()->setBlendEnabled(false);
        getGlobalState()->setDepthEnabled(false);
        getGlobalState()->setScissorEnabled(false);
        FF_GL_CALL(glBlitFramebuffer, 0, 0, _presentationRenderTarget->getWidth(), _presentationRenderTarget->getHeight(),
            0, 0, _presentationRenderTarget->getWidth(), _presentationRenderTarget->getHeight(), // back buffer guaranteed to be the same dimensions as presentation render target
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST);

        // Explicitly synchronize. This is the end of the render so we
        // don't need to worry about stalling anymore. In the future,
        // I'd like to replace this with triple-buffering and
        // fence synchronization, but for simple games I think this
        // will work fine.
        // 
        // Without glFinish, next frame might come around, and since
        // buffers have the unsynchronized flag, there is a chance
        // that the buffers will be overwritten with data before
        // the previous frame has finished drawing. Doesn't happen
        // on all devices (doesn't glitch on S7 or Nexus 7 2013, but
        // does on Pixel 4A). I'm guessing it has to do with processor
        // speed, but could also be EGL implementation on the driver
        // level.
        //
        // We explicitly synchronize like this when using Metal, so
        // I'm fine with this solution for now. Triple buffering
        // can be added to both backends at the same time.
        // @todo Triple buffering so we don't stall
        FF_GL_CALL(glFinish);
        
        // Not strictly correct because we haven't called `glFinish` but
        // it's close enough to capture the heavy work
        // UPDATE: We're calling glFinish now, but this needs to be
        // kept in mind in case it gets removed.
        ff::Locator::getStatistics().endStopwatch("game render");
    }

    void GLGraphicsDevice::onBackBufferDimensionsUpdated(const glm::ivec2& dimensions) {
        // In GL 4.0, the back buffer can't be written to at the same time as a color attachment.
        // GLES does support this. This behavior limits Metal, so we need a workaround. Instead
        // of rendering directly to the back buffer, we're going to render to a render target
        // instead, then blit to the back buffer.
        // 
        // Ultimately, we're going to use Vulkan/DX12 (haven't decided) for non-macOS platforms
        // so performance isn't an issue (not that this method is slow anyway for a desktop GPU).
        _presentationRenderTarget = createTexture<ColorTexture>(ColorBufferFormat::RGBA8Unorm,
            dimensions.x, dimensions.y,
            0,
            TextureUsage::GPU_RENDER_TARGET
            | TextureUsage::GPU_LOAD
            | TextureUsage::GPU_STORE
            | TextureUsage::GPU_BLIT,
            "BACK_BUFFER_PRE_TARGET");

        if (_readPFBO == 0) {
            FF_GL_CALL(glGenFramebuffers, 1, &_readPFBO);
        }
        // @todo Cleanup, support renderbuffer case. There's also no reason this needs to be a custom FBO, switch to FBO wrapper.
        FF_ASSERT(static_cast<GLTextureImp*>(_presentationRenderTarget->getImp())->getGLTextureType() == GLTextureType::TEXTURE,
            "Presentation render target is a renderbuffer, not a texture.");
        getGlobalState()->bindReadFramebuffer(_readPFBO);
        FF_GL_CALL(glFramebufferTexture2D,
            GL_READ_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            static_cast<GLTextureImp*>(_presentationRenderTarget->getImp())->getObject(),
            0);
}

    TextureImp* GLGraphicsDevice::createColorTextureImp(TextureFlag_t const& flags,
        TextureUsage_t const& usage) {
        return new GLTextureImp(this);
    }
    TextureImp* GLGraphicsDevice::createDepthTextureImp(TextureFlag_t const& flags,
        TextureUsage_t const& usage) {
        return new GLTextureImp(this);
    }
    std::shared_ptr<ISamplerState> GLGraphicsDevice::createSamplerStateImpl(const SamplerFilter& minFilter,
        const SamplerFilter& magFilter,
        const SamplerAddressMode& sAddressMode,
        const SamplerAddressMode& tAddressMode,
        const SamplerMipFilter& mipFilter) {
        return std::make_shared<GLSamplerState>(minFilter, magFilter, sAddressMode, tAddressMode, mipFilter);
    }

    std::shared_ptr<IVertexDescriptor> GLGraphicsDevice::createVertexDescriptorImpl(const VertexDescriptorInfo& info) {
        return std::static_pointer_cast<IVertexDescriptor>(std::make_shared<GLVertexDescriptor>(this, info));
    }

    std::shared_ptr<IRenderState> GLGraphicsDevice::createRenderStateImpl(std::shared_ptr<IShader> const& shader,
        std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
        std::vector<RenderColorLayerState> const& renderColorLayerStates,
        std::optional<DepthBufferFormat> const& depthBufferFormat) {
        return std::make_shared<GLRenderState>(this,
            shader,
            vertexDescriptor,
            renderColorLayerStates,
            depthBufferFormat);
    }
    std::shared_ptr<IDepthStencilState> GLGraphicsDevice::createDepthStencilStateImpl(const DepthCompareFunction& depthCompareFunction,
        const bool& depthWriteEnabled) {
        return std::make_shared<GLDepthStencilState>(this, depthCompareFunction, depthWriteEnabled);
    }

    std::shared_ptr<IRenderPass> GLGraphicsDevice::beginRenderPassImpl(std::vector<std::pair<ResourceHandle<ColorTexture>,
                RenderTargetColorAttachmentOptions>> const& colorTargets,
            std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget) {
        if (!_hasRenderedToPresentationTarget) {
            for (auto& pair : colorTargets) {
                if (pair.first == _presentationRenderTarget) {
                    _hasRenderedToPresentationTarget = true;
                    break;
                }
            }
        }
        return std::static_pointer_cast<IRenderPass>(std::make_shared<GLRenderPass>(this, colorTargets, depthTarget));
    }
    std::shared_ptr<IBlitPass> GLGraphicsDevice::beginBlitPassImpl() {
        return std::make_shared<GLBlitPass>(this);
    }

    std::shared_ptr<IShader> GLGraphicsDevice::getShaderImpl(const std::string& shaderName) {
        return _shaderLibrary->getShader(shaderName);
    }

    IndexBufferImp* GLGraphicsDevice::createIndexBufferImp(IndexType const& type, const int& elementSize, const int& count) {
        return new GLIndexBufferImp(this, elementSize, count);
    }
    UniformBufferImp* GLGraphicsDevice::createUniformBufferImp(const int& elementSize) {
        return new GLUniformBufferImp(this, elementSize);
    }
    VertexBufferImp* GLGraphicsDevice::createVertexBufferImp(const int& elementSize, const int& count) {
        return new GLVertexBufferImp(this, elementSize, count);
    }

    void GLGraphicsDevice::initializeShaderLibrary(ff::IAssetBundle* const& assetBundle) {
        _shaderLibrary = assetBundle->load<GLShaderLibrary>(std::string("shader_library_main") + (isGLES() ? "_opengl_es" : "_opengl"));
    }
    void GLGraphicsDevice::collectInfo() {
        FF_CONSOLE_LOG("Retrieving graphics device info...");

        _vendorStr = (char const*)FF_GL_CALL(glGetString, GL_VENDOR);
        FF_CONSOLE_LOG("OpenGL Vendor: %s", _vendorStr);
        _rendererStr = (char const*)FF_GL_CALL(glGetString, GL_RENDERER);
        FF_CONSOLE_LOG("OpenGL Renderer: %s", _rendererStr);
        _versionStr = (char const*)FF_GL_CALL(glGetString, GL_VERSION);
        FF_CONSOLE_LOG("OpenGL Version: %s", _versionStr);

#if defined(FF_GL_ES)
        _isES = true;
#else
        _isES = false;
#endif
        FF_CONSOLE_LOG("Is OpenGl ES? %s", _isES ? "Yes" : "No");

        GLint numExtensions = 0;
        FF_GL_CALL(glGetIntegerv, GL_NUM_EXTENSIONS, &numExtensions);
        if (numExtensions > 0) {
            FF_CONSOLE_LOG("%s extensions available:", numExtensions);
            for (GLint i = 0; i < numExtensions; i++) {
                const char* extensionName = (const char*)glGetStringi(GL_EXTENSIONS, i);
                bool usingExtension = false;
                // @todo Make into a restration system to reduce copy-paste error
                // @todo CVar for using extensions
                if (CVars::get<bool>("graphics_gl_use_arb_draw_buffer_blend")
                    && strcmp(extensionName, "GL_ARB_draw_buffers_blend") == 0) {
                    _extensions |= GLExtensions::ARB_DRAW_BUFFERS_BLEND;
                    usingExtension = true;
                }
                if (CVars::get<bool>("graphics_gl_use_arb_texture_storage")
                    && strcmp(extensionName, "GL_ARB_texture_storage") == 0) {
                    _extensions |= GLExtensions::ARB_TEXTURE_STORAGE;
                    usingExtension = true;
                }
                if (CVars::get<bool>("graphics_gl_use_arb_invalidate_subdata")
                    && strcmp(extensionName, "GL_ARB_invalidate_subdata") == 0) {
                    _extensions |= GLExtensions::ARB_INVALIDATE_SUBDATA;
                    usingExtension = true;
                }
                if (CVars::get<bool>("graphics_gl_use_arb_debug_label")
                    && strcmp(extensionName, "GL_ARB_debug_label") == 0) {
                    _extensions |= GLExtensions::ARB_DEBUG_LABEL;
                    usingExtension = true;
                }
                if (CVars::get<bool>("graphics_gl_use_khr_debug")
                    && strcmp(extensionName, "GL_KHR_debug") == 0) {
                    _extensions |= GLExtensions::KHR_DEBUG;
                    usingExtension = true;
                }
                //KHR_DEBUG

                FF_CONSOLE_LOG("\t - %s (%s)", extensionName, usingExtension ? "using" : "not using");

            }
        }
        else {
            FF_CONSOLE_LOG("No extensions supported.");
        }

        GLint textureUnits;
        FF_GL_CALL(glGetIntegerv, GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnits);
        _maxTextureUnits = (int)textureUnits;
        FF_CONSOLE_LOG("Available texture units: %s", textureUnits);

        GLint maxTextureSize;
        FF_GL_CALL(glGetIntegerv, GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        _maxTextureSize = (int)maxTextureSize;
        FF_CONSOLE_LOG("Maximum texture size: %s", maxTextureSize);

        GLint maxDrawBuffers;
        FF_GL_CALL(glGetIntegerv, GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);
        _maxDrawBuffers = (int)maxDrawBuffers;
        FF_CONSOLE_LOG("Maximum draw buffers: %s", _maxDrawBuffers);
        FF_ASSERT(_maxDrawBuffers >= 4, "Max draw buffers must be at least 4 (OpenGL driver is non-compliant)."); // @todo Change to user-aware assert (error dialog).

        GLint maxLabelLength = -1;
        if ((getExtensions() & GLExtensions::ARB_DEBUG_LABEL) > 0) {
            FF_GL_CALL(glGetIntegerv, GL_MAX_LABEL_LENGTH, &maxLabelLength);
        }
#if defined(FF_GL_HAS_GL_OBJECT_LABEL_KHR)
        if ((getExtensions() & GLExtensions::KHR_DEBUG) > 0) {
            FF_GL_CALL(glGetIntegerv, GL_MAX_LABEL_LENGTH_KHR, &maxLabelLength);
        }
#endif
        if (maxLabelLength < 0) {
            FF_CONSOLE_WARN("Context does not not support GL_ARB_debug_label/GL_KHR_debug.");
        }
        else {
            FF_CONSOLE_LOG("Max label length: %s", maxLabelLength);
        }
        _maxLabelLength = maxLabelLength;
    }
}
