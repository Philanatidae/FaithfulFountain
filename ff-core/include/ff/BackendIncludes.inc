/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Ensure that epoxy gets included first, in case
// other backends try to load OpenGL themselves.
#if defined(FF_HAS_GRAPHICS_OPENGL)
#include <ff-graphics-opengl/GLGraphicsDevice.hpp>
#endif
#if defined(FF_HAS_GRAPHICS_METAL)
#include <ff-graphics-metal/MetalGraphicsDevice.hmm>
#endif

#if defined(FF_HAS_AUDIO_COREAUDIO)
#include <ff-audio-coreaudio/CAAudioBackend.hmm>
#endif
#if defined(FF_HAS_AUDIO_PORTAUDIO)
#include <ff-audio-portaudio/PAAudioBackend.hpp>
#endif
#if defined(FF_HAS_AUDIO_OBOE)
#include <ff-audio-oboe/OboeAudioBackend.hpp>
#endif
