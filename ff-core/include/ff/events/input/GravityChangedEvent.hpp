/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_EVENTS_INPUT_GRAVITY_CHANGED_EVENT_HPP
#define _FF_EVENTS_INPUT_GRAVITY_CHANGED_EVENT_HPP

#include <ff/messages/Event.hpp>
#include <tinyformat/tinyformat.h>

#include <glm/glm.hpp>

namespace ff {

    FF_EVENT_DEFINE_1(GravityChangedEvent,
        "evt_motion_gravity_changed",
        glm::vec3, gravity,
        "<%s, %s, %s>",
        gravity.x, gravity.y, gravity.z);

}

#endif

