/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_BACKEND_SELECTOR_HPP
#define _FAITHFUL_FOUNTAIN_BACKEND_SELECTOR_HPP

#include <unordered_map>
#include <string>
#include <functional>

#include <ff/BackendIncludes.inc>
#include <ff/Console.hpp>
#include <ff/CVars.hpp>

namespace ff {

void provideBackend(std::string const& cvarName,
    std::string const& defaultBackend,
    std::unordered_map<std::string, std::function<void()>>& backends) {
    std::string& backend = CVars::get<std::string>(cvarName);
    if(backend != "default" && backends.find(backend) == backends.end()) {
        FF_CONSOLE_ERROR("`%s` is not a valid/available backend for `%s`. Switching to `default`.",
            backend,
            cvarName);
        backend = defaultBackend;
    }

    if(backend == "default") {
        FF_CONSOLE_LOG("Providing default backend for `%s` (`%s`).",
            cvarName,
            defaultBackend);
        backends[defaultBackend]();
    } else {
        FF_CONSOLE_LOG("Providing `%s` backend for `%s`.",
            backend,
            cvarName);
        if(defaultBackend != backend) {
            // @todo If not FF_DEV_FEATURES, have a pop-up to warn
            // the user that this is not a tested backend. Also, maybe
            // have an option to only have platform-preferred backends
            // enabled when doing a production build.
            FF_CONSOLE_WARN("`%s` is not preferred for this platform (preferred backend is `%s`).",
                backend,
                defaultBackend);
        }
        backends[backend]();
    }
}

void provideBackends() {
    // Backends are in order of preference.

    //=== GRAPHICS ===
    FF_CONSOLE_LOG("Graphics backends available:");
    std::string defaultGraphicsBackend;
    std::unordered_map<std::string, std::function<void()>> graphicsBackends; 

#if defined(FF_HAS_GRAPHICS_METAL)
    FF_CONSOLE_LOG("\t - `metal`");
    if(defaultGraphicsBackend.size() == 0) {
        defaultGraphicsBackend = "metal";
    }
    graphicsBackends["metal"] = []() -> void {
        Locator::provide(new MetalGraphicsDevice());
    };
#endif
#if defined(FF_HAS_GRAPHICS_OPENGL)
    FF_CONSOLE_LOG("\t - `opengl`");
    if(defaultGraphicsBackend.size() == 0) {
        defaultGraphicsBackend = "opengl";
    }
    graphicsBackends["opengl"] = []() -> void {
        // @todo The backend format needs to be able to be specified by the environment.
        // In fact, it's usually not even RGBA8, it's RGBA8_SRGBA, so
        // that's something we'll have to figure out at some point.
        Locator::provide(new GLGraphicsDevice(GL_RGBA8, -1));
    };
#endif
    // Don't advertise `null`
    if(defaultGraphicsBackend.size() == 0) {
        defaultGraphicsBackend = "null";
    }
    graphicsBackends["null"] = []() -> void {
        Locator::provide(new NullGraphicsDevice());
    };

    if(graphicsBackends.size() > 0) {
        provideBackend("graphics_backend",
            defaultGraphicsBackend,
            graphicsBackends);
    } else {
        FF_CONSOLE_WARN("No graphics backends available.");
    }

    //=== AUDIO ===
    FF_CONSOLE_LOG("Audio backends available:");
    std::string defaultAudioBackend;
    std::unordered_map<std::string, std::function<void()>> audioBackends; 

#if defined(FF_HAS_AUDIO_COREAUDIO)
    FF_CONSOLE_LOG("\t - `coreaudio`");
    if(defaultAudioBackend.size() == 0) {
        defaultAudioBackend = "coreaudio";
    }
    audioBackends["coreaudio"] = []() -> void {
        Locator::provide(new CAAudioBackend());
    };
#endif
#if defined(FF_HAS_AUDIO_OBOE)
    FF_CONSOLE_LOG("\t - `oboe`");
    if(defaultAudioBackend.size() == 0) {
        defaultAudioBackend = "oboe";
    }
    audioBackends["oboe"] = []() -> void {
        Locator::provide(new OboeAudioBackend());
    };
#endif
#if defined(FF_HAS_AUDIO_PORTAUDIO)
    FF_CONSOLE_LOG("\t - `portaudio`");
    if(defaultAudioBackend.size() == 0) {
        defaultAudioBackend = "portaudio";
    }
    audioBackends["portaudio"] = []() -> void {
        Locator::provide(new PAAudioBackend());
    };
#endif
    // Don't advertise `null`
    if(defaultAudioBackend.size() == 0) {
        defaultAudioBackend = "null";
    }
    audioBackends["portaudio"] = []() -> void {
        Locator::provide(new NullAudioBackend());
    };

    if(audioBackends.size() > 0) {
        provideBackend("audio_backend",
            defaultAudioBackend,
            audioBackends);
    } else {
        FF_CONSOLE_WARN("No audio backends available.");
    }
}

}

#endif
