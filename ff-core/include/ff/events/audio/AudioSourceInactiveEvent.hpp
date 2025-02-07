/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_AUDIO_AUDIO_SOURCE_INACTIVE_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_AUDIO_AUDIO_SOURCE_INACTIVE_EVENT_HPP

#include <ff/messages/Event.hpp>
#include <ff/audio/AudioSource.hpp>
#include <memory>
#include <tinyformat/tinyformat.h>

namespace ff {
    FF_EVENT_DEFINE_1(AudioSourceInactiveEvent,
        "evt_audio_source_inactive",
        std::shared_ptr<AudioSource>, audioSource,
        "%s",
        audioSource.get());
}

#endif
