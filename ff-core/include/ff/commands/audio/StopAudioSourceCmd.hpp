/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMMANDS_AUDIO_STOP_AUDIO_SOURCE_COMMAND_HPP
#define _FAITHFUL_FOUNTAIN_COMMANDS_AUDIO_STOP_AUDIO_SOURCE_COMMAND_HPP

#include <ff/messages/CmdHelpers.hpp>

#include <ff/audio/AudioSource.hpp>
#include <tinyformat/tinyformat.h>
#include <memory>

namespace ff {
    FF_CMD_DEFINE_1_R0(StopAudioSourceCmd,
        "cmd_stop_audio_source",
        "Stop an AudioSource",
        std::shared_ptr<AudioSource>, audioSource,
        "%s",
        (audioSource.get()));
}

#endif
