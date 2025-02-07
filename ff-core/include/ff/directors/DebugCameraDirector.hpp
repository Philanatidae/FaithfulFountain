/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_DIRECTORS_DEBUG_CAMERA_DIRECTOR_HPP
#define _FF_DIRECTORS_DEBUG_CAMERA_DIRECTOR_HPP

#include <ff/processes/Process.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff/events/input/MouseButtonDownEvent.hpp>
#include <ff/events/input/MouseButtonUpEvent.hpp>
#include <ff/events/input/MouseMoveEvent.hpp>
#include <ff/events/input/KeyboardKeyDownEvent.hpp>
#include <ff/events/input/KeyboardKeyUpEvent.hpp>

#include <ff/graphics/PerspectiveCamera.hpp>

#include <set>

namespace ff {

class DebugCameraDirector final : public Process,
    public EventListener<MouseButtonDownEvent>,
    public EventListener<MouseButtonUpEvent>,
    public EventListener<MouseMoveEvent>,
    public EventListener<KeyboardKeyDownEvent>,
    public EventListener<KeyboardKeyUpEvent> {
public:
    DebugCameraDirector(std::string const& camera);

    bool processEvent(MouseButtonDownEvent const& evt) override;
    bool processEvent(MouseButtonUpEvent const& evt) override;
    bool processEvent(MouseMoveEvent const& evt) override;
    bool processEvent(KeyboardKeyDownEvent const& evt) override;
    bool processEvent(KeyboardKeyUpEvent const& evt) override;

protected:
    void onInitialize() override;
    void onUpdate(const float& dt) override;

private:
    std::string _camera;

    bool _isMoving;
    std::set<KeyboardKey> _hadPress;

    glm::vec3 _relVel;
    glm::vec2 _relMouseSum;
};

}

#endif

