/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/CVars.hpp>

FF_CVAR_DEFINE(audio_oboe_app_sample_rate, int, -1, ff::CVarFlags::NONE, "Sample rate of the android app as reported by the audio manager.")
FF_CVAR_DEFINE(audio_oboe_app_frames_per_burst, int, -1, ff::CVarFlags::NONE, "Frames per burst of the android app as reported by the audio manager.")
