/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-sdl2/SDLWindow.hpp>

#include <ff/messages/MessageBus.hpp>

#include <ff/CVars.hpp>
#include <ff/Locator.hpp>
#include <ff/Console.hpp>

#include <ff/input/Keyboard.hpp>
#include <ff/events/input/KeyboardKeyDownEvent.hpp>
#include <ff/events/input/KeyboardKeyUpEvent.hpp>

#include <ff/input/Mouse.hpp>
#include <ff/events/input/MouseMoveEvent.hpp>
#include <ff/events/input/MouseButtonDownEvent.hpp>
#include <ff/events/input/MouseButtonUpEvent.hpp>
#include <ff/events/input/MouseScrollEvent.hpp>

#include <ff/events/env/EnvTextInputCharacterEvent.hpp>

#include <ff/events/ResizeEvent.hpp>

#include <ff-sdl2/directors/DesktopWindowAspectRatioMaintainerDirector.hpp>

namespace ff {

SDLWindow::SDLWindow(Game* const& game,
    int const& width,
    int const& height,
    std::string const& title)
    :_windowPtr(nullptr, [](SDL_Window* ptr) {}),
    _mousePos(0, 0) {
    uint32_t windowFlags = SDL_WINDOW_SHOWN
        | SDL_WINDOW_ALLOW_HIGHDPI;
#if defined(FF_HAS_GRAPHICS_OPENGL)
    if(Locator::getGraphicsDevice().getBackendName() == GraphicsBackend::OPENGL) {
        windowFlags |= SDL_WINDOW_OPENGL;
    }
#endif

    // @todo Turn `resizable` to live CVar
    if(CVars::get<bool>("window_resizable")) {
        windowFlags |= SDL_WINDOW_RESIZABLE;
    }

    if(CVars::get<bool>("window_force_initial_aspect_ratio")) {
        // @todo If able (likely platform dependent), force aspect
        // ratio with the native window. Else, fall back to the
        // director.
        game->attachProcess(std::make_shared<DesktopWindowAspectRatioMaintainerDirector>(width / (float)height));
    }

    _windowPtr = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(SDL_CreateWindow(title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        windowFlags),
            [](SDL_Window* ptr) -> void {
        if(ptr != nullptr) {
            SDL_DestroyWindow(ptr);
        }
    });
    // NOTE: SDL disables the dock and menu bar. If this is an issue, we will
    // need a fork, or maybe mess with NSApplication flags ourselves.
    // https://github.com/libsdl-org/SDL/blob/f1d9c36136b9d880cc9865c9e989c7310b4ad243/src/video/cocoa/SDL_cocoawindow.m#L1165
    if(_windowPtr == nullptr) {
        FF_CONSOLE_FATAL("Unable to create SDL window: %s", SDL_GetError());
    }

#if defined(FF_HAS_GRAPHICS_OPENGL)
    if(Locator::getGraphicsDevice().getBackendName() == GraphicsBackend::OPENGL) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
            4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
            0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,
            1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE,
            8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,
            8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,
            8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,
            8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,
            0);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,
            0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
            SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

        _glContext = SDL_GL_CreateContext(_windowPtr.get());
        if(_glContext == nullptr) {
            FF_CONSOLE_FATAL("Unable to create SDL OpenGL context: %s", SDL_GetError());
        }

        SDL_GL_MakeCurrent(_windowPtr.get(), _glContext);
    }
#endif

    // Ready for engine events
    Locator::getMessageBus().addHandler<ResizeWindowCmd>(this);
    Locator::getMessageBus().addHandler<AbsoluteMouseMovementCmd>(this);
    Locator::getMessageBus().addHandler<RelativeMouseMovementCmd>(this);

    // Initialize gamepad events
    _gamepadManager.init();
}
SDLWindow::~SDLWindow() {
#if defined(FF_HAS_GRAPHICS_OPENGL)
    if(_glContext != nullptr) {
        SDL_GL_DeleteContext(_glContext);
        _glContext = nullptr;
    }
#endif
}

bool SDLWindow::isValid() const {
    return _windowPtr != nullptr;
}

bool SDLWindow::getShouldClose() {
    return _shouldClose;
}

void SDLWindow::show() {
    SDL_ShowWindow(_windowPtr.get());
}
void SDLWindow::hide() {
    SDL_HideWindow(_windowPtr.get());
}

void SDLWindow::setTitle(std::string const& title) {
    SDL_SetWindowTitle(_windowPtr.get(), title.c_str());
}

glm::ivec2 SDLWindow::getFrameBufferSize() const {
    glm::ivec2 size(0, 0);
    SDL_GetWindowSizeInPixels(_windowPtr.get(),
        &size.x, &size.y);
    return size;
}
double SDLWindow::getRefreshRate() const {
    // @todo Verify that this works
    SDL_DisplayMode mode;
    if(SDL_GetWindowDisplayMode(_windowPtr.get(), &mode) != 0) {
        return -1;
    }
    return mode.refresh_rate;
}

SDL_Window* SDLWindow::getSDLWindowPtr() const {
    return _windowPtr.get();
}

void SDLWindow::pollEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        // @todo Support for multiple windows

        switch(event.type) {
            case SDL_QUIT:
                _shouldClose = true;
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                handleEventKey(event);
                break;

            case SDL_MOUSEMOTION:
                handleEventMouseMotion(event);
                break;
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
                handleEventMouseButton(event);
                break;
            case SDL_MOUSEWHEEL:
                handleEventMouseWheel(event);
                break;

            case SDL_TEXTINPUT:
                handleEventTextInput(event);
                break;
            case SDL_TEXTEDITING:
                handleEventTextEditing(event);
                break;

            case SDL_WINDOWEVENT:
                if(event.window.windowID == SDL_GetWindowID(_windowPtr.get())) {
                    handleEventWindow(event);
                }
                break;

            case SDL_CONTROLLERDEVICEADDED:
            case SDL_CONTROLLERDEVICEREMOVED:
                _gamepadManager.handleEventControllerDevice(event);
                break;

            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                _gamepadManager.handleEventControllerButton(event);
                break;
            case SDL_CONTROLLERAXISMOTION:
                _gamepadManager.handleEventControllerAxis(event);
                break;

            default:
                break;
        }
    }
}

std::unique_ptr<typename ResizeWindowCmd::Ret> SDLWindow::handleCmd(ResizeWindowCmd const& cmd) {
    // FF works in pixels, but SDL windows are sized in logical units.
    // On HDPI screens, pixels may be 2x the window pixels. We need
    // to do a conversion.
    glm::ivec2 const prevFramebuffer = getFrameBufferSize();
    glm::ivec2 prevWindowSize;
    SDL_GetWindowSize(_windowPtr.get(), &prevWindowSize.x, &prevWindowSize.y);
    glm::vec2 const scaling(prevFramebuffer.x / (float)prevWindowSize.x,
        prevFramebuffer.y / (float)prevWindowSize.y);
    // @todo Epsilon sanity check
    SDL_SetWindowSize(_windowPtr.get(),
        (int)(cmd.width * scaling.x),
        (int)(cmd.height * scaling.y));

    return std::make_unique<typename ResizeWindowCmd::Ret>();
}
std::unique_ptr<typename AbsoluteMouseMovementCmd::Ret> SDLWindow::handleCmd(AbsoluteMouseMovementCmd const& cmd) {
    SDL_SetRelativeMouseMode(SDL_FALSE);

    return std::make_unique<typename AbsoluteMouseMovementCmd::Ret>();
}
std::unique_ptr<typename RelativeMouseMovementCmd::Ret> SDLWindow::handleCmd(RelativeMouseMovementCmd const& cmd) {
    SDL_SetRelativeMouseMode(SDL_TRUE);

    return std::make_unique<typename RelativeMouseMovementCmd::Ret>();
}

void SDLWindow::handleEventKey(SDL_Event& event) {
    ff::KeyboardKey engineKey = ff::KeyboardKey::UNKNOWN;

    if(event.key.repeat > 0) {
        return;
    }

    switch(event.key.keysym.sym) {
        case SDLK_SPACE:
            engineKey = ff::KeyboardKey::SPACE;
            break;
        case SDLK_QUOTE:
            engineKey = ff::KeyboardKey::APOSTROPHE;
            break;
        case SDLK_COMMA:
            engineKey = ff::KeyboardKey::COMMA;
            break;
        case SDLK_MINUS:
            engineKey = ff::KeyboardKey::MINUS;
            break;
        case SDLK_PERIOD:
            engineKey = ff::KeyboardKey::PERIOD;
            break;
        case SDLK_SLASH:
            engineKey = ff::KeyboardKey::SLASH;
            break;

        case SDLK_0:
            engineKey = ff::KeyboardKey::ZERO;
            break;
        case SDLK_1:
            engineKey = ff::KeyboardKey::ONE;
            break;
        case SDLK_2:
            engineKey = ff::KeyboardKey::TWO;
            break;
        case SDLK_3:
            engineKey = ff::KeyboardKey::THREE;
            break;
        case SDLK_4:
            engineKey = ff::KeyboardKey::FOUR;
            break;
        case SDLK_5:
            engineKey = ff::KeyboardKey::FIVE;
            break;
        case SDLK_6:
            engineKey = ff::KeyboardKey::SIX;
            break;
        case SDLK_7:
            engineKey = ff::KeyboardKey::SEVEN;
            break;
        case SDLK_8:
            engineKey = ff::KeyboardKey::EIGHT;
            break;
        case SDLK_9:
            engineKey = ff::KeyboardKey::NINE;
            break;
        
        case SDLK_a:
            engineKey = ff::KeyboardKey::A;
            break;
        case SDLK_b:
            engineKey = ff::KeyboardKey::B;
            break;
        case SDLK_c:
            engineKey = ff::KeyboardKey::C;
            break;
        case SDLK_d:
            engineKey = ff::KeyboardKey::D;
            break;
        case SDLK_e:
            engineKey = ff::KeyboardKey::E;
            break;
        case SDLK_f:
            engineKey = ff::KeyboardKey::F;
            break;
        case SDLK_g:
            engineKey = ff::KeyboardKey::G;
            break;
        case SDLK_h:
            engineKey = ff::KeyboardKey::H;
            break;
        case SDLK_i:
            engineKey = ff::KeyboardKey::I;
            break;
        case SDLK_j:
            engineKey = ff::KeyboardKey::J;
            break;
        case SDLK_k:
            engineKey = ff::KeyboardKey::K;
            break;
        case SDLK_l:
            engineKey = ff::KeyboardKey::L;
            break;
        case SDLK_m:
            engineKey = ff::KeyboardKey::M;
            break;
        case SDLK_n:
            engineKey = ff::KeyboardKey::N;
            break;
        case SDLK_o:
            engineKey = ff::KeyboardKey::O;
            break;
        case SDLK_p:
            engineKey = ff::KeyboardKey::P;
            break;
        case SDLK_q:
            engineKey = ff::KeyboardKey::Q;
            break;
        case SDLK_r:
            engineKey = ff::KeyboardKey::R;
            break;
        case SDLK_s:
            engineKey = ff::KeyboardKey::S;
            break;
        case SDLK_t:
            engineKey = ff::KeyboardKey::T;
            break;
        case SDLK_u:
            engineKey = ff::KeyboardKey::U;
            break;
        case SDLK_v:
            engineKey = ff::KeyboardKey::V;
            break;
        case SDLK_w:
            engineKey = ff::KeyboardKey::W;
            break;
        case SDLK_x:
            engineKey = ff::KeyboardKey::X;
            break;
        case SDLK_y:
            engineKey = ff::KeyboardKey::Y;
            break;
        case SDLK_z:
            engineKey = ff::KeyboardKey::Z;
            break;

        case SDLK_LEFTBRACKET:
            engineKey = ff::KeyboardKey::LEFT_BRACKET;
            break;
        case SDLK_RIGHTBRACKET:
            engineKey = ff::KeyboardKey::RIGHT_BRACKET;
            break;
        case SDLK_BACKSLASH:
            engineKey = ff::KeyboardKey::BACKSLASH;
            break;
        case SDLK_BACKQUOTE:
            engineKey = ff::KeyboardKey::BACKTICK;
            break;
        case SDLK_SEMICOLON:
            engineKey = ff::KeyboardKey::SEMICOLON;
            break;
        case SDLK_EQUALS:
            engineKey = ff::KeyboardKey::EQUAL;
            break;
        
        case SDLK_ESCAPE:
            engineKey = ff::KeyboardKey::ESCAPE;
            break;
        case SDLK_RETURN:
            engineKey = ff::KeyboardKey::ENTER;
            break;
        case SDLK_TAB:
            engineKey = ff::KeyboardKey::TAB;
            break;
        case SDLK_BACKSPACE:
            engineKey = ff::KeyboardKey::BACKSPACE;
            break;
        case SDLK_INSERT:
            engineKey = ff::KeyboardKey::INSERT;
            break;
        case SDLK_DELETE:
            engineKey = ff::KeyboardKey::DEL;
            break;
        case SDLK_RIGHT:
            engineKey = ff::KeyboardKey::RIGHT;
            break;
        case SDLK_LEFT:
            engineKey = ff::KeyboardKey::LEFT;
            break;
        case SDLK_UP:
            engineKey = ff::KeyboardKey::UP;
            break;
        case SDLK_DOWN:
            engineKey = ff::KeyboardKey::DOWN;
            break;
        case SDLK_PAGEUP:
            engineKey = ff::KeyboardKey::PAGE_UP;
            break;
        case SDLK_PAGEDOWN:
            engineKey = ff::KeyboardKey::PAGE_DOWN;
            break;
        case SDLK_HOME:
            engineKey = ff::KeyboardKey::HOME;
            break;
        case SDLK_END:
            engineKey = ff::KeyboardKey::END;
            break;
        case SDLK_PAUSE:
            engineKey = ff::KeyboardKey::PAUSE;
            break;

        case SDLK_F1:
            engineKey = ff::KeyboardKey::PAUSE;
            engineKey = ff::KeyboardKey::F1;
            break;
        case SDLK_F2:
            engineKey = ff::KeyboardKey::F2;
            break;
        case SDLK_F3:
            engineKey = ff::KeyboardKey::F3;
            break;
        case SDLK_F4:
            engineKey = ff::KeyboardKey::F4;
            break;
        case SDLK_F5:
            engineKey = ff::KeyboardKey::F5;
            break;
        case SDLK_F6:
            engineKey = ff::KeyboardKey::F6;
            break;
        case SDLK_F7:
            engineKey = ff::KeyboardKey::F7;
            break;
        case SDLK_F8:
            engineKey = ff::KeyboardKey::F8;
            break;
        case SDLK_F9:
            engineKey = ff::KeyboardKey::F9;
            break;
        case SDLK_F10:
            engineKey = ff::KeyboardKey::F10;
            break;
        case SDLK_F11:
            engineKey = ff::KeyboardKey::F11;
            break;
        case SDLK_F12:
            engineKey = ff::KeyboardKey::F12;
            break;
        case SDLK_F13:
            engineKey = ff::KeyboardKey::F13;
            break;
        // @todo Support up to F24
            
        case SDLK_KP_0:
            engineKey = ff::KeyboardKey::NUM_ZERO;
            break;
        case SDLK_KP_1:
            engineKey = ff::KeyboardKey::NUM_ONE;
            break;
        case SDLK_KP_2:
            engineKey = ff::KeyboardKey::NUM_TWO;
            break;
        case SDLK_KP_3:
            engineKey = ff::KeyboardKey::NUM_THREE;
            break;
        case SDLK_KP_4:
            engineKey = ff::KeyboardKey::NUM_FOUR;
            break;
        case SDLK_KP_5:
            engineKey = ff::KeyboardKey::NUM_FIVE;
            break;
        case SDLK_KP_6:
            engineKey = ff::KeyboardKey::NUM_SIX;
            break;
        case SDLK_KP_7:
            engineKey = ff::KeyboardKey::NUM_SEVEN;
            break;
        case SDLK_KP_8:
            engineKey = ff::KeyboardKey::NUM_EIGHT;
            break;
        case SDLK_KP_9:
            engineKey = ff::KeyboardKey::NUM_NINE;
            break;
        case SDLK_KP_DECIMAL:
            engineKey = ff::KeyboardKey::NUM_DECIMAL;
            break;
        case SDLK_KP_DIVIDE:
            engineKey = ff::KeyboardKey::NUM_DIVIDE;
            break;
        case SDLK_KP_MULTIPLY:
            engineKey = ff::KeyboardKey::NUM_MULTIPLY;
            break;
        case SDLK_KP_MINUS:
            engineKey = ff::KeyboardKey::NUM_SUBTRACT;
            break;
        case SDLK_KP_PLUS:
            engineKey = ff::KeyboardKey::NUM_ADD;
            break;
        case SDLK_KP_ENTER:
            engineKey = ff::KeyboardKey::NUM_ENTER;
            break;
        case SDLK_KP_EQUALS:
            engineKey = ff::KeyboardKey::NUM_EQUAL;
            break;
        
        case SDLK_LSHIFT:
            engineKey = ff::KeyboardKey::LEFT_SHIFT;
            break;
        case SDLK_RSHIFT:
            engineKey = ff::KeyboardKey::RIGHT_SHIFT;
            break;
        case SDLK_LCTRL:
            engineKey = ff::KeyboardKey::LEFT_CONTROL;
            break;
        case SDLK_RCTRL:
            engineKey = ff::KeyboardKey::RIGHT_CONTROL;
            break;
        case SDLK_LALT:
            engineKey = ff::KeyboardKey::LEFT_ALT;
            break;
        case SDLK_RALT:
            engineKey = ff::KeyboardKey::RIGHT_ALT;
            break;
        case SDLK_LGUI:
            engineKey = ff::KeyboardKey::LEFT_SUPER;
            break;
        case SDLK_RGUI:
            engineKey = ff::KeyboardKey::RIGHT_SUPER;
            break;
        default:
            break;
    }

    if(engineKey == ff::KeyboardKey::UNKNOWN) {
        FF_CONSOLE_WARN("Unknown mapping from SDL key (%s, %s)", event.key.keysym.sym, SDL_GetKeyName(event.key.keysym.sym));
        return;
    }

    ff::KeyboardModifiers_t engineMods(0);
    if((event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) != 0) {
        engineMods |= ff::KeyboardModifiers::SHIFT;
    }
    if((event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) != 0) {
        engineMods |= ff::KeyboardModifiers::CONTROL;
    }
    if((event.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) != 0) {
        engineMods |= ff::KeyboardModifiers::ALT;
    }
    if((event.key.keysym.mod & (KMOD_LGUI | KMOD_RGUI)) != 0) {
        engineMods |= ff::KeyboardModifiers::SUPER;
    }

    if(event.type == SDL_KEYDOWN) {
        ff::Locator::getMessageBus().dispatch<ff::KeyboardKeyDownEvent>(engineKey, engineMods);
    }
    if(event.type == SDL_KEYUP) {
        ff::Locator::getMessageBus().dispatch<ff::KeyboardKeyUpEvent>(engineKey, engineMods);
    }
}
void SDLWindow::handleEventMouseMotion(SDL_Event& event) {
    if(event.motion.which == SDL_TOUCH_MOUSEID) {
        return;
    }

    int width, height;
    SDL_GetWindowSize(_windowPtr.get(), &width, &height);

    if(SDL_GetRelativeMouseMode()) {
        _mousePos.x = _mousePos.y = 0;

        glm::vec2 relMousePos(event.motion.xrel / (float)width,
            event.motion.yrel / (float)height);
        Locator::getMessageBus().dispatch<ff::MouseMoveEvent>(relMousePos, MouseMoveType::RELATIVE);
    } else {
        _mousePos.x = glm::mix(-1.0f, 1.0f, event.motion.x / (float)width);
        _mousePos.y = glm::mix(-1.0f, 1.0f, 1 - event.motion.y / (float)height);
        Locator::getMessageBus().dispatch<ff::MouseMoveEvent>(_mousePos, MouseMoveType::ABSOLUTE);
    }
}
void SDLWindow::handleEventMouseButton(SDL_Event& event) {
    if(event.button.which == SDL_TOUCH_MOUSEID) {
        return;
    }

    MouseButton engineButton = MouseButton::UNKNOWN;

    switch(event.button.button) {
        case SDL_BUTTON_LEFT:
            engineButton = MouseButton::PRIMARY;
            break;
        case SDL_BUTTON_RIGHT:
            engineButton = MouseButton::SECONDARY;
            break;
        case SDL_BUTTON_MIDDLE:
            engineButton = MouseButton::MIDDLE;
            break;
        // @todo Support X1 & X2
        default:
            break;
    }

    if(engineButton == ff::MouseButton::UNKNOWN) {
        FF_CONSOLE_WARN("Unknown mapping from SDL mouse button (%s)",
            event.button.button);
        return;
    }

    if(event.type == SDL_MOUSEBUTTONDOWN) {
        Locator::getMessageBus().dispatch<MouseButtonDownEvent>(engineButton,
            _mousePos);
    }
    if(event.type == SDL_MOUSEBUTTONUP) {
        Locator::getMessageBus().dispatch<MouseButtonUpEvent>(engineButton,
            _mousePos);
    }
}
void SDLWindow::handleEventMouseWheel(SDL_Event& event) {
    if(event.wheel.which == SDL_TOUCH_MOUSEID) {
        return;
    }

    Locator::getMessageBus().dispatch<MouseScrollEvent>(glm::vec2(event.wheel.x,
        event.wheel.y));
}

void SDLWindow::handleEventTextInput(SDL_Event& event) {
    FF_ASSERT(strlen(event.text.text) == 1, "Multiple character input not yet implemented: `%s`", event.text.text);
    // FF uses UTF-8; the SDL event is in UTF-8. This differs from GLFW, which
    // uses UTF-32.
    // Of course, this is "supports" UTF-8, because with only one character sent
    // at a time, it will break as soon as there's a non-ASCII character.
    // @todo Engine doesn't properly support UTF-8. FIX BEFORE RELEASE!
    Locator::getMessageBus().dispatch<EnvTextInputCharacterEvent>(event.text.text[0]);
}
void SDLWindow::handleEventTextEditing(SDL_Event& event) {
    FF_CONSOLE_WARN("`SDL_TEXTEDITING` is not supported.");
}

void SDLWindow::handleEventWindow(SDL_Event& event) {
    switch(event.window.event) {
    case SDL_WINDOWEVENT_RESIZED:
        handleEventWindowResized(event.window);
        break;
    default:
        break;
    }
}
void SDLWindow::handleEventWindowResized(SDL_WindowEvent& event) {
    // SDL provides the window new size in this event, but ResizeEvent is in real pixels.
    glm::ivec2 framebufferSize = getFrameBufferSize();
    Locator::getMessageBus().dispatch<ResizeEvent>(framebufferSize.x,
        framebufferSize.y);
}

}
