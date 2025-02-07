/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/dev/ImGuiCore.hpp>

#include <ff/messages/MessageBus.hpp>
#include <ff/events/dev/PopulateImGuiFrameEvent.hpp>

#include <ff/commands/env/EnvCursorChangeCmd.hpp>
#include <ff/commands/env/EnvCopyToClipboardCmd.hpp>

#include <ff/commands/env/EnvBeginTextInputCmd.hpp>
#include <ff/commands/env/EnvEndTextInputCmd.hpp>

#include <ff/Locator.hpp>

#include <ImGuizmo.h>
#include <imgui_internal.h>

namespace ff {

ImGuiCore::ImGuiCore()
    :ff::Process(ProcessPriority::EXTREMELY_LOW),
    _previousCursor(-2),
    _nodesContext(nullptr, [](ImNodes::Ez::Context* context) -> void { }) {
}

std::unique_ptr<RenderImGuiCmd::Ret> ImGuiCore::handleCmd(RenderImGuiCmd const& cmd) {
    newFrame(cmd.dt);

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    Locator::getMessageBus().dispatch<PopulateImGuiFrameEvent>(cmd.debugCamera);
    ImGui::EndFrame();
    if(!ImGui::GetIO().WantCaptureMouse) {
        if(ImGui::IsMouseClicked(ImGuiMouseButton_Right)
            || ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
            ImGui::FocusWindow(nullptr);
        }
    }
    ImGui::Render();

    ImDrawData* drawData = ImGui::GetDrawData();
    if(drawData->TotalVtxCount > 0
        && drawData->TotalIdxCount > 0) {
        RenderTargetColorAttachmentOptions colorOptions;
        colorOptions.loadAction = RenderTargetLoadAction::LOAD;
        colorOptions.storeAction = RenderTargetStoreAction::STORE;

        auto pass = Locator::getGraphicsDevice().beginRenderPass({
                std::make_pair(Locator::getGraphicsDevice().getPresentationRenderTarget(), colorOptions)
        });

        pass->setState(_renderState);
        pass->setViewport(0, 0,
            drawData->DisplaySize.x * drawData->FramebufferScale.x,
            drawData->DisplaySize.y * drawData->FramebufferScale.y);
        pass->setCullMode(CullMode::NONE);
        pass->setUniformBuffer(_camera->getUniformBuffer(), 0);

        if(_vertexBuffer == nullptr
            || _vertexBuffer->getCount() < drawData->TotalVtxCount) {
            _vertexBuffer = Locator::getGraphicsDevice().createVertexBuffer<ImDrawVert>(drawData->TotalVtxCount);
        }
        if(_indexBuffer == nullptr
            || _indexBuffer->getCount() < drawData->TotalIdxCount) {
            _indexBuffer = Locator::getGraphicsDevice().createIndexBuffer<IndexType::UINT16>(drawData->TotalIdxCount);
        }

        pass->setVertexBuffer(_vertexBuffer, 0);

        glm::ivec2 const clip_off((int)drawData->DisplayPos.x, (int)drawData->DisplayPos.y);
        glm::ivec2 const clip_scale((int)drawData->FramebufferScale.x, (int)drawData->FramebufferScale.y);

        int vertexBufferOffset = 0;
        int indexBufferOffset = 0;
        for(int i = 0; i < drawData->CmdListsCount; i++) {
            ImDrawList const* cmd_list = drawData->CmdLists[i];

            if(_vertexData.max_size() < cmd_list->VtxBuffer.size()) {
                _vertexData.reserve(cmd_list->VtxBuffer.size());
            }
            for(int i = 0; i < cmd_list->VtxBuffer.size(); i++) {
                _vertexData.push_back(cmd_list->VtxBuffer.Data[i]);
                glm::vec2 uv = Locator::getGraphicsDevice().processUVCoords(_vertexData[i].uv.x, _vertexData[i].uv.y);
                _vertexData[i].uv.x = uv.x;
                _vertexData[i].uv.y = uv.y;
            }
            _vertexBuffer->bufferData(_vertexData.data(), _vertexData.size(), vertexBufferOffset);
            _vertexData.clear();

            _indexBuffer->bufferData(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.size(), indexBufferOffset);

            for(int j = 0; j < cmd_list->CmdBuffer.size(); j++) {
                ImDrawCmd const* cmd = &cmd_list->CmdBuffer[j];
                if(cmd->UserCallback) {
                    // User callback, registered via ImDrawList::AddCallback()
                    // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                    if (cmd->UserCallback == ImDrawCallback_ResetRenderState) {
                        // @todo ???
                        // I don't really know what the point of this is.
                        //ImGui_ImplMetal_SetupRenderState(drawData, commandBuffer, commandEncoder, renderPipelineState, vertexBuffer, vertexBufferOffset);
                    } else {
                        cmd->UserCallback(cmd_list, cmd);
                    }
                } else {
                    if(cmd->ElemCount == 0) {
                        continue;
                    }

                    // Project scissor rectangle into framebuffer space
                    glm::ivec2 clip_min((int)((cmd->ClipRect.x - clip_off.x) * clip_scale.x),
                        (int)((cmd->ClipRect.y - clip_off.y) * clip_scale.y));
                    glm::ivec2 clip_max((int)((cmd->ClipRect.z - clip_off.x) * clip_scale.x),
                        (int)((cmd->ClipRect.w - clip_off.y) * clip_scale.y));

                    // Clamp to viewport
                    if(clip_min.x < 0) {
                        clip_min.x = 0;
                    }
                    if(clip_min.y < 0) {
                        clip_min.y = 0;
                    }
                    if(clip_max.x > Locator::getGraphicsDevice().getBackBufferDimensions().x) {
                        clip_max.x = Locator::getGraphicsDevice().getBackBufferDimensions().x;
                    }
                    if(clip_max.y > Locator::getGraphicsDevice().getBackBufferDimensions().y) {
                        clip_max.y = Locator::getGraphicsDevice().getBackBufferDimensions().y;
                    }
                    if(clip_max.x <= clip_min.x
                        || clip_max.y <= clip_min.y) {
                        continue;
                    }

                    pass->setScissorRect(clip_min.x, clip_min.y, clip_max.x - clip_min.x, clip_max.y - clip_min.y);

                    if(ImTextureID tex_id = cmd->GetTexID()) {
                        // Not great, but there isn't much choice. Assume that whoever set this holds ownership and will clean up correctly.
                        ResourceHandle<ColorTexture>* texture = reinterpret_cast<ResourceHandle<ColorTexture>*>(tex_id);
                        pass->setTexture(*texture, 0);
                        pass->setSamplerState(_fontSamplerState, 0);
                    }

                    pass->setVertexBufferOffset(0, (vertexBufferOffset + cmd->VtxOffset) * sizeof(ImDrawVert));
                    pass->drawIndexedPrimitives(RenderPrimitives::TRIANGLE_LIST,
                        _indexBuffer,
                        cmd->ElemCount,
                        indexBufferOffset + cmd->IdxOffset);
                }
            }

            vertexBufferOffset += cmd_list->VtxBuffer.Size;
            indexBufferOffset += cmd_list->IdxBuffer.Size;
        }

        pass->end();
    }

    return std::make_unique<RenderImGuiCmd::Ret>();
}

bool ImGuiCore::processEvent(ff::MouseButtonDownEvent const& evt) {
    if(ImGui::GetCurrentContext() == nullptr) {
        return false;
    }

    ImGuiMouseButton button = convertMouseButtonToImGuiMouseButton(evt.button);
    if(button < 0) {
        return false;
    }
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, true);

    return io.WantCaptureMouse;
}
bool ImGuiCore::processEvent(ff::MouseButtonUpEvent const& evt) {
    if(ImGui::GetCurrentContext() == nullptr) {
        return false;
    }

    ImGuiMouseButton button = convertMouseButtonToImGuiMouseButton(evt.button);
    if(button < 0) {
        return false;
    }
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, false);

    return io.WantCaptureMouse;
}
bool ImGuiCore::processEvent(ff::MouseMoveEvent const& evt) {
    if(ImGui::GetCurrentContext() == nullptr) {
        return false;
    }

    glm::vec4 mousePos = _camera->getCombinedInverse() * glm::vec4(evt.position, 0, 1);
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(mousePos.x, mousePos.y);

    return io.WantCaptureMouse;
}
bool ImGuiCore::processEvent(ff::MouseScrollEvent const& evt) {
    if(ImGui::GetCurrentContext() == nullptr) {
        return false;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent(evt.offset.x, evt.offset.y);

    return io.WantCaptureMouse;
}
bool ImGuiCore::processEvent(ff::KeyboardKeyDownEvent const& evt) {
    if(ImGui::GetCurrentContext() == nullptr) {
        return false;
    }

    ImGuiKey key = convertKeyToImGuiKey(evt.key);
    if(key == ImGuiKey_None) {
        return false;
    }
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(key, true);

    updateKeyModifiers(evt.mods);

    return io.WantCaptureKeyboard;
}
bool ImGuiCore::processEvent(ff::KeyboardKeyUpEvent const& evt) {
    if(ImGui::GetCurrentContext() == nullptr) {
        return false;
    }

    ImGuiKey key = convertKeyToImGuiKey(evt.key);
    if(key == ImGuiKey_None) {
        return false;
    }
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(key, false);

    updateKeyModifiers(evt.mods);

    return io.WantCaptureKeyboard;
}
bool ImGuiCore::processEvent(ff::EnvTextInputCharacterEvent const& evt) {
    if(ImGui::GetCurrentContext() == nullptr) {
        return false;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter((unsigned int)evt.c);
    
    return io.WantTextInput;
}
bool ImGuiCore::processEvent(ff::EnvTextInputCompletedEvent const& evt) {
    ImGuiIO& io = ImGui::GetIO();
    // @todo This is hacky.
    ImGui::SetWindowFocus(nullptr);

    return false;
}
bool ImGuiCore::processEvent(ff::ResizeEvent const& evt) {
    // @todo This '/ 2' is because of HDPI scaling. This should come from a CVar instead of being hardcoded.
    _camera->adjust(0, evt.width / 2.0f,
        0, evt.height / 2.0f,
        true);
    _camera->update();

    ImGuiIO& io = ImGui::GetIO(); 
    io.DisplaySize = ImVec2(evt.width / 2.0f,
        evt.height / 2.0f);

    return false;
}

void ImGuiCore::onInitialize() {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.UserData = this;
    io.BackendPlatformUserData = this;
    io.BackendPlatformName = "imgui_impl_ff";
    if((Locator::getEnvironment().getCapabilities() & EnvironmentCapabilities::MOUSE_CURSORS) != 0) {
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    }

    if(Locator::getEnvironment().getCapabilities() & EnvironmentCapabilities::CLIPBOARD) {
        io.ClipboardUserData = this;
        io.GetClipboardTextFn = [](void* user_data) -> const char* {
            ImGuiCore* core = (ImGuiCore*)user_data;
            core->_clipboard = Locator::getEnvironment().getClipboard();
            return core->_clipboard.c_str();
        };
        io.SetClipboardTextFn = [](void* user_data, const char* text) -> void {
            Locator::getMessageBus().dispatch<EnvCopyToClipboardCmd>(std::string(text));
        };
    }

    glm::ivec2 backbuffer = Locator::getGraphicsDevice().getBackBufferDimensions();
    _camera = std::make_unique<OrthographicCamera>(0, backbuffer.x / 2.0f,
        0, backbuffer.y / 2.0f,
        true);
    
    Locator::getMessageBus().addListener<MouseButtonDownEvent>(this, ff::MessageListenerPriority::EXTREMELY_HIGH + 1);
    Locator::getMessageBus().addListener<MouseButtonUpEvent>(this, ff::MessageListenerPriority::EXTREMELY_HIGH + 1);
    Locator::getMessageBus().addListener<MouseMoveEvent>(this, ff::MessageListenerPriority::EXTREMELY_HIGH + 1);
    Locator::getMessageBus().addListener<MouseScrollEvent>(this, ff::MessageListenerPriority::EXTREMELY_HIGH + 1); 
    Locator::getMessageBus().addListener<KeyboardKeyDownEvent>(this, ff::MessageListenerPriority::EXTREMELY_HIGH + 1);
    Locator::getMessageBus().addListener<KeyboardKeyUpEvent>(this, ff::MessageListenerPriority::EXTREMELY_HIGH + 1);
    Locator::getMessageBus().addListener<EnvTextInputCharacterEvent>(this, ff::MessageListenerPriority::EXTREMELY_HIGH + 1);
    Locator::getMessageBus().addListener<EnvTextInputCompletedEvent>(this, ff::MessageListenerPriority::EXTREMELY_HIGH + 1);
    Locator::getMessageBus().addListener<MouseButtonDownEvent>(this, ff::MessageListenerPriority::EXTREMELY_HIGH + 1);
    Locator::getMessageBus().addListener<ResizeEvent>(this);

    io.BackendRendererUserData = this;
    io.BackendRendererName = "imgui_impl_ff";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    ResourceHandle<TextureData> fontTexData = ResourceHandle<TextureData>::createResource([]() -> TextureData* {
        uint8_t* pixels;
        int width = -1, height = -1;
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        TextureFormat const format = TextureFormat::RGBA8Unorm;
        return new TextureData(pixels,
            BufferFormatInfo::get(format).size * width * height,
            width,
            height,
            format,
            false);
    });
    FF_ASSERT(Locator::getColorTextureManager().createTexture("font_imgui",
        convertTextureFormatToColorBufferFormat(fontTexData->getFormat()),
        fontTexData->getWidth(),
        fontTexData->getHeight(),
        0,
        TextureUsage::GPU_SAMPLE
            | TextureUsage::GPU_BLIT
            | TextureUsage::CPU_WRITE), "Could not create ImGUI Font texture.");
    _fontTexture = Locator::getColorTextureManager().getTexture("font_imgui");
    _fontTexture->bufferImage(fontTexData);

    io.Fonts->SetTexID((void*)&_fontTexture);
    _fontSamplerState = Locator::getGraphicsDevice().createSamplerState(SamplerFilter::LINEAR,
        SamplerFilter::LINEAR,
        SamplerAddressMode::REPEAT,
        SamplerAddressMode::REPEAT);

    _vertexDescriptor = Locator::getGraphicsDevice().createVertexDescriptor(VertexDescriptorInfo({
        BufferDescriptorInfo(0, sizeof(ImDrawVert), {
            AttributeDescriptorInfo(0, IM_OFFSETOF(ImDrawVert, pos), AttributeFormat::Float2),
            AttributeDescriptorInfo(1, IM_OFFSETOF(ImDrawVert, uv), AttributeFormat::Float2),
            AttributeDescriptorInfo(2, IM_OFFSETOF(ImDrawVert, col), AttributeFormat::UByte4),
        })
    }));

    _renderState = Locator::getGraphicsDevice().createRenderState(Locator::getGraphicsDevice().getShader("sprite_imgui"),
        _vertexDescriptor,
        {
        RenderColorLayerState(Locator::getGraphicsDevice().getPresentationRenderTarget()->getFormat(),
            BlendState(BlendOperation::Add,
                BlendFactor::SourceAlpha,
                BlendFactor::OneMinusSourceAlpha,
                
                BlendOperation::Add,
                BlendFactor::One,
                BlendFactor::OneMinusSourceAlpha))
        });

    FF_ASSERT(sizeof(ImDrawIdx) == 2, "ImGuiCore assumes sizeof(ImDrawIdx) to be '2'.");

    _nodesContext = std::unique_ptr<ImNodes::Ez::Context, void(*)(ImNodes::Ez::Context*)>(ImNodes::Ez::CreateContext(), [](ImNodes::Ez::Context* context) -> void {
        if(context != nullptr) {
            ImNodes::Ez::FreeContext(context);
        }
    });

    // Ready to render
    Locator::getMessageBus().addHandler<RenderImGuiCmd>(this);
}
void ImGuiCore::onKill() {
    ImGui::DestroyContext();
}

ImGuiKey ImGuiCore::convertKeyToImGuiKey(KeyboardKey const& key) {
    switch(key) {
    case KeyboardKey::SPACE:
        return ImGuiKey_Space;
    case KeyboardKey::APOSTROPHE:
        return ImGuiKey_Apostrophe;
    case KeyboardKey::COMMA:
        return ImGuiKey_Comma;
    case KeyboardKey::MINUS:
        return ImGuiKey_Minus;
    case KeyboardKey::PERIOD:
        return ImGuiKey_Period;
    case KeyboardKey::SLASH:
        return ImGuiKey_Slash;

    case KeyboardKey::ZERO:
        return ImGuiKey_0;
    case KeyboardKey::ONE:
        return ImGuiKey_1;
    case KeyboardKey::TWO:
        return ImGuiKey_2;
    case KeyboardKey::THREE:
        return ImGuiKey_3;
    case KeyboardKey::FOUR:
        return ImGuiKey_4;
    case KeyboardKey::FIVE:
        return ImGuiKey_5;
    case KeyboardKey::SIX:
        return ImGuiKey_6;
    case KeyboardKey::SEVEN:
        return ImGuiKey_7;
    case KeyboardKey::EIGHT:
        return ImGuiKey_8;
    case KeyboardKey::NINE:
        return ImGuiKey_9;

    case KeyboardKey::A:
        return ImGuiKey_A;
    case KeyboardKey::B:
        return ImGuiKey_B;
    case KeyboardKey::C:
        return ImGuiKey_C;
    case KeyboardKey::D:
        return ImGuiKey_D;
    case KeyboardKey::E:
        return ImGuiKey_E;
    case KeyboardKey::F:
        return ImGuiKey_F;
    case KeyboardKey::G:
        return ImGuiKey_G;
    case KeyboardKey::H:
        return ImGuiKey_H;
    case KeyboardKey::I:
        return ImGuiKey_I;
    case KeyboardKey::J:
        return ImGuiKey_J;
    case KeyboardKey::K:
        return ImGuiKey_K;
    case KeyboardKey::L:
        return ImGuiKey_L;
    case KeyboardKey::M:
        return ImGuiKey_M;
    case KeyboardKey::N:
        return ImGuiKey_N;
    case KeyboardKey::O:
        return ImGuiKey_O;
    case KeyboardKey::P:
        return ImGuiKey_P;
    case KeyboardKey::Q:
        return ImGuiKey_Q;
    case KeyboardKey::R:
        return ImGuiKey_R;
    case KeyboardKey::S:
        return ImGuiKey_S;
    case KeyboardKey::T:
        return ImGuiKey_T;
    case KeyboardKey::U:
        return ImGuiKey_U;
    case KeyboardKey::V:
        return ImGuiKey_V;
    case KeyboardKey::W:
        return ImGuiKey_W;
    case KeyboardKey::X:
        return ImGuiKey_X;
    case KeyboardKey::Y:
        return ImGuiKey_Y;
    case KeyboardKey::Z:
        return ImGuiKey_Z;

    case KeyboardKey::LEFT_BRACKET:
        return ImGuiKey_LeftBracket;
    case KeyboardKey::RIGHT_BRACKET:
        return ImGuiKey_RightBracket;
    case KeyboardKey::BACKSLASH:
        return ImGuiKey_Backslash;
    case KeyboardKey::BACKTICK:
        return ImGuiKey_GraveAccent;
    case KeyboardKey::SEMICOLON:
        return ImGuiKey_Semicolon;
    case KeyboardKey::EQUAL:
        return ImGuiKey_Equal;

    case KeyboardKey::ESCAPE:
        return ImGuiKey_Escape;
    case KeyboardKey::ENTER:
        return ImGuiKey_Enter;
    case KeyboardKey::TAB:
        return ImGuiKey_Tab;
    case KeyboardKey::BACKSPACE:
        return ImGuiKey_Backspace;
    case KeyboardKey::INSERT:
        return ImGuiKey_Insert;
    case KeyboardKey::DEL:
        return ImGuiKey_Delete;
    case KeyboardKey::RIGHT:
        return ImGuiKey_RightArrow;
    case KeyboardKey::LEFT:
        return ImGuiKey_LeftArrow;
    case KeyboardKey::DOWN:
        return ImGuiKey_DownArrow;
    case KeyboardKey::UP:
        return ImGuiKey_UpArrow;
    case KeyboardKey::PAGE_UP:
        return ImGuiKey_PageUp;
    case KeyboardKey::PAGE_DOWN:
        return ImGuiKey_PageDown;
    case KeyboardKey::HOME:
        return ImGuiKey_Home;
    case KeyboardKey::END:
        return ImGuiKey_End;
    case KeyboardKey::PAUSE:
        return ImGuiKey_Pause;

    case KeyboardKey::F1:
        return ImGuiKey_F1;
    case KeyboardKey::F2:
        return ImGuiKey_F2;
    case KeyboardKey::F3:
        return ImGuiKey_F3;
    case KeyboardKey::F4:
        return ImGuiKey_F4;
    case KeyboardKey::F5:
        return ImGuiKey_F5;
    case KeyboardKey::F6:
        return ImGuiKey_F6;
    case KeyboardKey::F7:
        return ImGuiKey_F7;
    case KeyboardKey::F8:
        return ImGuiKey_F8;
    case KeyboardKey::F9:
        return ImGuiKey_F9;
    case KeyboardKey::F10:
        return ImGuiKey_F10;
    case KeyboardKey::F11:
        return ImGuiKey_F11;
    case KeyboardKey::F12:
        return ImGuiKey_F12;

    case KeyboardKey::NUM_ZERO:
        return ImGuiKey_Keypad0;
    case KeyboardKey::NUM_ONE:
        return ImGuiKey_Keypad1;
    case KeyboardKey::NUM_TWO:
        return ImGuiKey_Keypad2;
    case KeyboardKey::NUM_THREE:
        return ImGuiKey_Keypad3;
    case KeyboardKey::NUM_FOUR:
        return ImGuiKey_Keypad4;
    case KeyboardKey::NUM_FIVE:
        return ImGuiKey_Keypad5;
    case KeyboardKey::NUM_SIX:
        return ImGuiKey_Keypad6;
    case KeyboardKey::NUM_SEVEN:
        return ImGuiKey_Keypad7;
    case KeyboardKey::NUM_EIGHT:
        return ImGuiKey_Keypad8;
    case KeyboardKey::NUM_NINE:
        return ImGuiKey_Keypad9;
    case KeyboardKey::NUM_DECIMAL:
        return ImGuiKey_KeypadDecimal;
    case KeyboardKey::NUM_DIVIDE:
        return ImGuiKey_KeypadDivide;
    case KeyboardKey::NUM_MULTIPLY:
        return ImGuiKey_KeypadMultiply;
    case KeyboardKey::NUM_SUBTRACT:
        return ImGuiKey_KeypadSubtract;
    case KeyboardKey::NUM_ADD:
        return ImGuiKey_KeypadAdd;
    case KeyboardKey::NUM_ENTER:
        return ImGuiKey_KeypadEnter;
    case KeyboardKey::NUM_EQUAL:
        return ImGuiKey_KeypadEqual;

    case KeyboardKey::LEFT_SHIFT:
        return ImGuiKey_LeftShift;
    case KeyboardKey::RIGHT_SHIFT:
        return ImGuiKey_RightShift;
    case KeyboardKey::LEFT_CONTROL:
        return ImGuiKey_LeftCtrl;
    case KeyboardKey::RIGHT_CONTROL:
        return ImGuiKey_RightCtrl;
    case KeyboardKey::LEFT_ALT:
        return ImGuiKey_LeftAlt;
    case KeyboardKey::RIGHT_ALT:
        return ImGuiKey_RightAlt;
    case KeyboardKey::LEFT_SUPER:
        return ImGuiKey_LeftSuper;
    case KeyboardKey::RIGHT_SUPER:
        return ImGuiKey_RightSuper;

    default:
        return ImGuiKey_None;
    }
}
ImGuiMouseButton ImGuiCore::convertMouseButtonToImGuiMouseButton(MouseButton const& button) {
    switch(button) {
    case MouseButton::PRIMARY:
        return ImGuiMouseButton_Left;
    case MouseButton::SECONDARY:
        return ImGuiMouseButton_Right;
    case MouseButton::MIDDLE:
        return ImGuiMouseButton_Middle;
    default:
        return -1;
    }
}

void ImGuiCore::updateKeyModifiers(KeyboardModifiers_t const& mods) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGuiKey_ModCtrl, (mods & KeyboardModifiers::CONTROL) != 0);
    io.AddKeyEvent(ImGuiKey_ModShift, (mods & KeyboardModifiers::SHIFT) != 0);
    io.AddKeyEvent(ImGuiKey_ModAlt, (mods & KeyboardModifiers::ALT) != 0);
    io.AddKeyEvent(ImGuiKey_ModSuper, (mods & KeyboardModifiers::SUPER) != 0);
}

void ImGuiCore::newFrame(float const& dt) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(ff::Locator::getGraphicsDevice().getBackBufferDimensions().x / 2.0f,
        ff::Locator::getGraphicsDevice().getBackBufferDimensions().y / 2.0f);
    io.DisplayFramebufferScale = ImVec2(2.0f, 2.0f); // @todo Change to CVar
    io.DeltaTime = dt;

    if(io.WantTextInput) {
        if(!Locator::getEnvironment().isRecordingTextInput()) {
            Locator::getMessageBus().dispatch<EnvBeginTextInputCmd>("");
        }
    } else {
        if(Locator::getEnvironment().isRecordingTextInput()) {
            Locator::getMessageBus().dispatch<EnvEndTextInputCmd>();
        }
    }

    if(!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
        ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
        if(cursor != _previousCursor) {
            _previousCursor = cursor;
            if(cursor == ImGuiMouseCursor_None) {
                FF_CONSOLE_ERROR("Cursor hiding is not supported."); // @todo Support cursor hiding
            } else {
                MouseCursor engineCursor(MouseCursor::UNKNOWN);
                switch(cursor) {
                case ImGuiMouseCursor_Arrow:
                    engineCursor = MouseCursor::DEFAULT;
                    break;
                case ImGuiMouseCursor_TextInput:
                    engineCursor = MouseCursor::IBEAM;
                    break;
                case ImGuiMouseCursor_ResizeNS:
                    engineCursor = MouseCursor::RESIZE_NS;
                    break;
                case ImGuiMouseCursor_ResizeEW:
                    engineCursor = MouseCursor::RESIZE_EW;
                    break;
                case ImGuiMouseCursor_Hand:
                    engineCursor = MouseCursor::HAND;
                    break;
                case ImGuiMouseCursor_ResizeAll:
                    engineCursor = MouseCursor::RESIZE_ALL;
                    break;
                case ImGuiMouseCursor_ResizeNESW:
                    engineCursor = MouseCursor::RESIZE_NESW;
                    break;
                case ImGuiMouseCursor_ResizeNWSE:
                    engineCursor = MouseCursor::RESIZE_NWSE;
                    break;
                case ImGuiMouseCursor_NotAllowed:
                    engineCursor = MouseCursor::NOT_ALLOWED;
                    break;
                default:
                    engineCursor = MouseCursor::UNKNOWN;
                }

                if(engineCursor != MouseCursor::UNKNOWN) {
                    Locator::getMessageBus().dispatch<EnvCursorChangeCmd>(engineCursor);
                }
            }
        }
    }
}

}
