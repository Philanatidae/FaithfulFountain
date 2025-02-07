/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_DEV_IM_GUI_CORE_HPP
#define _FAITHFUL_FOUNTAIN_DEV_IM_GUI_CORE_HPP

#include <ff/processes/Process.hpp>
#include <ff/processes/Process.hpp>

#include <imgui.h>
#include <ff/input/Keyboard.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff/events/input/MouseButtonDownEvent.hpp>
#include <ff/events/input/MouseButtonUpEvent.hpp>
#include <ff/events/input/MouseMoveEvent.hpp>
#include <ff/events/input/MouseScrollEvent.hpp>
#include <ff/events/input/KeyboardKeyDownEvent.hpp>
#include <ff/events/input/KeyboardKeyUpEvent.hpp>
#include <ff/events/env/EnvTextInputCharacterEvent.hpp>
#include <ff/events/env/EnvTextInputCompletedEvent.hpp>
#include <ff/events/ResizeEvent.hpp>

#include<ff/messages/CmdHandler.hpp>
#include <ff/commands/render/RenderImGuiCmd.hpp>

#include <ff/graphics/OrthographicCamera.hpp>

#include <ff/graphics/Texture.hpp>
#include <ff/graphics/ISamplerState.hpp>
#include <ff/graphics/IVertexDescriptor.hpp>
#include <ff/graphics/IRenderState.hpp>
#include <ff/graphics/VertexBuffer.hpp>
#include <ff/graphics/IndexBuffer.hpp>

#include <vector>

#include <imnodes/ImNodesEz.h>

namespace ff {

class ImGuiCore final : public ff::Process,
    public CmdHandler<RenderImGuiCmd>,
    public EventListener<MouseButtonDownEvent>,
    public EventListener<MouseButtonUpEvent>,
    public EventListener<MouseMoveEvent>,
    public EventListener<MouseScrollEvent>,
    public EventListener<KeyboardKeyDownEvent>,
    public EventListener<KeyboardKeyUpEvent>,
    public EventListener<EnvTextInputCharacterEvent>,
    public EventListener<EnvTextInputCompletedEvent>,
    public EventListener<ResizeEvent> {
public:
    ImGuiCore();
    ~ImGuiCore() = default;

    std::unique_ptr<RenderImGuiCmd::Ret> handleCmd(RenderImGuiCmd const& cmd) override;

    bool processEvent(ff::MouseButtonDownEvent const& evt) override;
    bool processEvent(ff::MouseButtonUpEvent const& evt) override;
    bool processEvent(ff::MouseMoveEvent const& evt) override;
    bool processEvent(ff::MouseScrollEvent const& evt) override;
    bool processEvent(ff::KeyboardKeyDownEvent const& evt) override;
    bool processEvent(ff::KeyboardKeyUpEvent const& evt) override;
    bool processEvent(ff::EnvTextInputCharacterEvent const& evt) override;
    bool processEvent(ff::EnvTextInputCompletedEvent const& evt) override;
    bool processEvent(ff::ResizeEvent const& evt) override;
protected:
    void onInitialize() override;
    void onKill() override;

private:
    std::string _clipboard;
    ImGuiMouseCursor _previousCursor;
    std::unique_ptr<OrthographicCamera> _camera;

    ResourceHandle<ColorTexture> _fontTexture;
    std::shared_ptr<ISamplerState> _fontSamplerState;
    std::shared_ptr<IVertexDescriptor> _vertexDescriptor;
    std::shared_ptr<IRenderState> _renderState;
    std::shared_ptr<VertexBuffer<ImDrawVert>> _vertexBuffer;
    std::vector<ImDrawVert> _vertexData;
    std::shared_ptr<IndexBuffer<IndexType::UINT16>> _indexBuffer;
    std::vector<IndexBuffer<IndexType::UINT16>::type> _indexData;

    std::unique_ptr<ImNodes::Ez::Context, void(*)(ImNodes::Ez::Context*)> _nodesContext;

    ImGuiKey convertKeyToImGuiKey(KeyboardKey const& key);
    ImGuiMouseButton convertMouseButtonToImGuiMouseButton(MouseButton const& button);

    void updateKeyModifiers(KeyboardModifiers_t const& mods);

    void newFrame(float const& dt);
};

}

#endif
