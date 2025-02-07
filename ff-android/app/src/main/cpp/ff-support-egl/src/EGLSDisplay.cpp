/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-support-egl/EGLSDisplay.hpp>

#include <vector>
#include <array>

#include <ff/Console.hpp>

namespace ff {
    EGLSDisplay EGLSDisplay::fromDefaultDisplay() {
        EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if(eglInitialize(display, nullptr, nullptr) == EGL_FALSE) {
            EGLenum error = eglGetError();
            FF_EGL_PRINT_FRIENDLY_ERROR(error);
            if(error == EGL_NOT_INITIALIZED) {
                FF_CONSOLE_ERROR("EGLDisplay `%s` could not be initialized.");
            }
        }
        return EGLSDisplay(display);
    }

    EGLSDisplay::EGLSDisplay(EGLDisplay display)
        :_display(display) {
    }
    EGLSDisplay::~EGLSDisplay() {
        destroy();
    }

    EGLSDisplay::EGLSDisplay(EGLSDisplay&& other)
            :_display(other._display) {
        other._display = EGL_NO_DISPLAY;
    }
    EGLSDisplay& EGLSDisplay::operator=(EGLSDisplay&& other) noexcept {
        _display = other._display;

        other._display = EGL_NO_DISPLAY;

        return *this;
    }

    EGLDisplay EGLSDisplay::getEGLDisplay() const {
        return _display;
    }

    bool EGLSDisplay::good() const {
        return !bad();
    }
    bool EGLSDisplay::bad() const {
        return _display == EGL_NO_DISPLAY;
    }

    void EGLSDisplay::destroy(EGLenum* const& errorOut) {
        if(_display != EGL_NO_DISPLAY) {
            if(eglTerminate(_display) == EGL_FALSE) {
                EGLenum error = eglGetError();
                if(errorOut != nullptr) {
                    *errorOut = error;
                }
                FF_EGL_PRINT_FRIENDLY_ERROR(error);
            }
            _display = EGL_NO_DISPLAY;
        }
    }

    std::vector<EGLSConfig> EGLSDisplay::getConfigs(EGLenum* const& errorOut) const {
        if(_display == EGL_NO_DISPLAY) {
            if(errorOut != nullptr) {
                *errorOut = EGL_BAD_DISPLAY;
            }
            return std::vector<EGLSConfig>();
        }

        std::vector<EGLConfig> configs;
        EGLint numConfigs;
        if(eglGetConfigs(_display, nullptr, 0, &numConfigs) == EGL_FALSE) {
            EGLenum errorCode = eglGetError();
            FF_EGL_PRINT_FRIENDLY_ERROR(errorCode);
            if(errorOut != nullptr) {
                *errorOut = errorCode;
            }
            return std::vector<EGLSConfig>();
        }
        configs.resize(numConfigs);
        eglGetConfigs(_display, configs.data(), numConfigs, &numConfigs); // Error was checked earlier

        std::vector<EGLSConfig> supportConfigs;
        configs.reserve(numConfigs);
        for(const auto& config : configs) {
            configs.emplace_back(config);
        }

        return supportConfigs;
    }
    std::vector<EGLSConfig> EGLSDisplay::getConfigs(EGLSConfigAttribList const& attribList, EGLenum* const& errorOut) const {
        if(_display == EGL_NO_DISPLAY) {
            if(errorOut != nullptr) {
                *errorOut = EGL_BAD_DISPLAY;
            }
            return std::vector<EGLSConfig>();
        }

        std::vector<EGLConfig> configs;
        EGLint numConfigs;
        if(eglChooseConfig(_display, attribList.getEGLConfigAttribList(), nullptr, 0, &numConfigs) == EGL_FALSE) {
            EGLenum errorCode = eglGetError();
            FF_EGL_PRINT_FRIENDLY_ERROR(errorCode);
            if(errorOut != nullptr) {
                *errorOut = errorCode;
            }
            return std::vector<EGLSConfig>();
        }
        configs.resize(numConfigs);
        eglChooseConfig(_display, attribList.getEGLConfigAttribList(), configs.data(), numConfigs, &numConfigs); // Error was checked earlier

        std::vector<EGLSConfig> supportConfigs;
        supportConfigs.reserve(numConfigs);
        for(const auto& config : configs) {
            supportConfigs.emplace_back(_display, config);
        }

        return supportConfigs;
    }

    EGLSContext EGLSDisplay::createContext(EGLSConfig const& config, EGLint const& clientVersion, EGLenum* const& errorOut) {
        if(_display == EGL_NO_DISPLAY) {
            if(errorOut != nullptr) {
                *errorOut = EGL_BAD_DISPLAY;
            }
            return std::move(EGLSContext(_display, EGL_NO_CONTEXT));
        }
        std::array<EGLint, 3> attribs({
            EGL_CONTEXT_CLIENT_VERSION, clientVersion,
            EGL_NONE
        });
        EGLContext context = eglCreateContext(_display, config.getEGLConfig(), nullptr, attribs.data());
        EGLSContext supportContext(_display, context);
        return std::move(supportContext);
    }
    EGLSSurface EGLSDisplay::createSurface(EGLSConfig const& config, EGLNativeWindowType window, EGLenum* const& errorOut) {
        if(_display == EGL_NO_DISPLAY) {
            if(errorOut != nullptr) {
                *errorOut = EGL_BAD_DISPLAY;
            }
            return std::move(EGLSSurface(_display, EGL_NO_SURFACE));
        }
        EGLSurface surface = eglCreateWindowSurface(_display, config.getEGLConfig(), window, nullptr);
        EGLSSurface supportSurface(_display, surface);
        return std::move(supportSurface);
    }

    bool EGLSDisplay::bind(EGLSSurface& drawSurface, EGLSSurface& readSurface, EGLSContext& context, EGLenum* const& errorOut) {
        if(_display == EGL_NO_DISPLAY) {
            if(errorOut != nullptr) {
                *errorOut = EGL_BAD_DISPLAY;
            }
            return false;
        }

        if(eglMakeCurrent(_display, drawSurface.getEGLSurface(), readSurface.getEGLSurface(), context.getEGLContext()) == EGL_FALSE) {
            EGLenum error = eglGetError();
            if(errorOut != nullptr) {
                *errorOut = error;
            }
            if(error == EGL_BAD_DISPLAY) {
                // Display has been lost
                drawSurface.destroy();
                readSurface.destroy();
                context.destroy();
                destroy();
            }
            if(error == EGL_BAD_CONTEXT
                || error == EGL_CONTEXT_LOST) {
                // Context has been lost
                context.destroy();
            }
            if(error == EGL_BAD_SURFACE) {
                // Surface has been lost
                if(drawSurface.getEGLSurface() == readSurface.getEGLSurface()) {
                    drawSurface.destroy();
                    readSurface.destroy();
                } else {
                    EGLenum error = EGL_SUCCESS;
                    drawSurface.queryConfigID(&error);
                    if(error != EGL_SUCCESS) {
                        drawSurface.destroy();
                    }
                    error = EGL_SUCCESS;
                    readSurface.queryConfigID(&error);
                    if(error != EGL_SUCCESS) {
                        readSurface.destroy();
                    }
                }
            }
            return false;
        }
        return true;
    }
    bool EGLSDisplay::bind(EGLSSurface& surface, EGLSContext& context, EGLenum* const& errorOut) {
        return bind(surface, surface, context, errorOut);
    }
    bool EGLSDisplay::unbind(EGLenum* const& errorOut) {
        if(_display == EGL_NO_DISPLAY) {
            if(errorOut != nullptr) {
                *errorOut = EGL_BAD_DISPLAY;
            }
            return false;
        }

        if(eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) == EGL_FALSE) {
            EGLenum error = eglGetError();
            if(errorOut != nullptr) {
                *errorOut = error;
            }
            if(error == EGL_BAD_DISPLAY) {
                // Display has been lost
                destroy();
            }
            return false;
        }
        return true;
    }

    bool EGLSDisplay::swapBuffers(EGLSSurface& surface, EGLSContext& context, EGLenum* const& errorOut) {
        if(_display == EGL_NO_DISPLAY) {
            if(errorOut != nullptr) {
                *errorOut = EGL_BAD_DISPLAY;
            }
            return false;
        }

        if(eglSwapBuffers(_display, surface.getEGLSurface()) == EGL_FALSE) {
            EGLenum error = eglGetError();
            if(errorOut != nullptr) {
                *errorOut = error;
            }
            if(error == EGL_BAD_DISPLAY) {
                // Display has been lost
                surface.destroy();
                context.destroy();
                destroy();
            }
            if(error == EGL_BAD_SURFACE) {
                // Surface has been lost
                surface.destroy();
            }
            if(error == EGL_BAD_CONTEXT
                || error == EGL_CONTEXT_LOST) {
                // Context has been lost
                surface.destroy();
            }
            return false;
        }
        return true;
    }
}
