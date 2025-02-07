/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <android_native_app_glue.h>
#include <android/native_activity.h>
#include <android/window.h>
#include <jni.h>

#include <epoxy/gl.h>

#include <EGL/egl.h> // @todo Change to epoxy?

#include <memory>

#include <ff/GameServicer.hpp>
#include <ff/messages/MessageBus.hpp>
#include <ff/commands/GameShutdownCommand.hpp>
#include <ff/events/ResizeEvent.hpp>
#include "AndroidGameLoop.hpp"
#include "AndroidEnvironment.hpp"
#include "LogcatConsole.hpp"
#include "AndroidAssetBundle.hpp"
#include <ff/events/env/EnvPauseRequestedEvent.hpp>
#include <ff/commands/env/EnvPrepareForSuspendCommand.hpp>
#include <ff/commands/env/EnvPrepareForRestoreCommand.hpp>

#include <ff/math/Rectangle.hpp>

#include <ff/events/input/MouseMoveEvent.hpp>
#include <ff/events/input/MouseButtonUpEvent.hpp>
#include <ff/events/input/MouseButtonDownEvent.hpp>

#include <ff/Locator.hpp>
#include <ff-graphics-opengl/GLGraphicsDevice.hpp>
#include <ff-audio-oboe/OboeAudioBackend.hpp>

#include <ff/graphics/Color.hpp>

#include <ff-support-egl/EGLSDisplay.hpp>
#include <ff-support-egl/EGLSContext.hpp>
#include <ff-support-egl/EGLSSurface.hpp>
#include <ff-support-egl/EGLSConfig.hpp>
#include <ff-support-egl/EGLSConfigAttribList.hpp>

#include <ff-graphics-opengl/gl.hpp>

#include <ff/BackendProvider.inc>

struct AppData {
    struct android_app* app;
    JNIEnv* jni;

    bool initialized;
    ff::CommandLineOptions commandLineOptions;
    ff::GameServicer* gameServicer;
    ff::AndroidEnvironment* environment;

    ff::EGLSDisplay display;
    ff::EGLSConfig config;
    ff::EGLSSurface surface;
    ff::EGLSContext context;

    EGLint width;
    EGLint height;
};

void clearScreen(AppData* appData) {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void swapBuffers(AppData* appData) {
    if(appData->display.bad()) {
        return;
    }
    appData->display.swapBuffers(appData->surface, appData->context);
}
void reinitSurface(AppData* appData);
void initDisplay(AppData* const& appData) {
    if(appData->display.bad()) {
        appData->display = ff::EGLSDisplay::fromDefaultDisplay();
        FF_ASSERT(appData->display.good(), "Could not create EGL default display.");
    }

    // Searching for a config with RGBA == 8 and has a depth buffer.
    ff::EGLSConfigAttribList attribs = ff::EGLSConfigAttribList::begin()
        .setRenderableType(ff::EGLSRenderableTypes::OPENGL_ES)
        .setSurfaceType(ff::EGLSSurfaceTypes::WINDOW)
        .setRedSize(8).setGreenSize(8).setBlueSize(8).setAlphaSize(8)
        .end();

    // @todo We can actually save config with the display
    eglBindAPI(EGL_OPENGL_ES_API);
    std::vector<ff::EGLSConfig> supportedConfigs = appData->display.getConfigs(attribs);
    if(supportedConfigs.size() == 0) {
        FF_CONSOLE_ERROR("Could not find any configs with the requested attributes.");
    }

    int i = 0;
    for(; i < supportedConfigs.size(); i++) {
        auto& cfg = supportedConfigs[i];

        if(cfg.getConfigAttribute(EGL_RED_SIZE) == 8
            && cfg.getConfigAttribute(EGL_BLUE_SIZE) == 8
            && cfg.getConfigAttribute(EGL_GREEN_SIZE) == 8
            && cfg.getConfigAttribute(EGL_ALPHA_SIZE) == 8
            && cfg.getConfigAttribute(EGL_DEPTH_SIZE) > 0) {
            appData->config = supportedConfigs[i];
            break;
        }
    }
    if(i == supportedConfigs.size()) {
        // Couldn't find an ideal one, pick the first available
        appData->config = supportedConfigs[0];
    }

    if(appData->config.bad()) {
        FF_CONSOLE_LOG("Unable to initialize EGLConfig");
        return;
    }

    reinitSurface(appData);

    if(appData->context.bad()) {
        appData->context = appData->display.createContext(appData->config, 3);
        FF_CONSOLE_LOG("New EGL context was created.");
    }

    EGLenum error;
    if(!appData->display.bind(appData->surface, appData->context, &error)) {
        FF_EGL_PRINT_FRIENDLY_ERROR(error);
        FF_CONSOLE_FATAL("Unable to bind surface & context.");
        return;
    }

    clearScreen(appData);
    swapBuffers(appData);
}
void termDisplay(AppData* appData, bool preserveContext) {
    // Discussion on Google Groups about NDK context preservation: https://groups.google.com/g/android-ndk/c/jwVMF6zINus
    // Note that this isn't recommended. Ideally, you'd destroy all assets
    // and re-create them on resume. But because Android isn't my primary target,
    // I don't care.

    if(appData->display.good()) {
        appData->display.unbind();
        if(appData->context.good()) {
            if(preserveContext) {
                // Don't destroy
            } else {
                appData->context.destroy();
            }
        }
        if(appData->surface.good()) {
            appData->surface.destroy();
        }
        if(!preserveContext) {
            appData->display.destroy();
            appData->config.destroy();
        }
    }
}
void reinitSurface(AppData* appData) {
    if(appData->display.bad()) {
        FF_CONSOLE_ERROR("EGLDisplay is not valid.");
        return;
    }
    if(appData->config.bad()) {
        FF_CONSOLE_ERROR("EGL config is not valid.");
        return;
    }

    if(appData->surface.good()) {
        appData->display.unbind();
        appData->surface.destroy();
    }

    appData->surface = appData->display.createSurface(appData->config, appData->app->window);
    appData->width = appData->surface.queryWidth();
    appData->height = appData->surface.queryHeight();
    appData->display.bind(appData->surface, appData->context);

    ff::Locator::getMessageBus().dispatch<ff::ResizeEvent>(appData->surface.queryWidth(),
        appData->surface.queryHeight());
}

void setWindowToFullscreen(AppData* appData) {
    jclass activityClass = appData->jni->GetObjectClass(appData->app->activity->clazz);

    jmethodID setToImmersiveMode = appData->jni->GetMethodID(activityClass, "setToImmersiveMode", "()V");

    appData->jni->CallVoidMethod(appData->app->activity->clazz, setToImmersiveMode);

    appData->jni->DeleteLocalRef(activityClass);
}
ff::Rectangle getSafeArea(AppData* appData) {
    jclass activityClass = appData->jni->GetObjectClass(appData->app->activity->clazz);
    jmethodID getSafeArea = appData->jni->GetMethodID(activityClass, "getSafeArea", "()Landroid/graphics/Rect;");

    jobject rect = appData->jni->CallObjectMethod(appData->app->activity->clazz, getSafeArea);

    jclass rectClass = appData->jni->FindClass("android/graphics/Rect");
    jfieldID leftFieldID = appData->jni->GetFieldID(rectClass, "left", "I");
    jfieldID rightFieldID = appData->jni->GetFieldID(rectClass, "right", "I");
    jfieldID topFieldID = appData->jni->GetFieldID(rectClass, "top", "I");
    jfieldID bottomFieldID = appData->jni->GetFieldID(rectClass, "bottom", "I");

    jint left = appData->jni->GetIntField(rect, leftFieldID);
    jint right = appData->jni->GetIntField(rect, rightFieldID);
    jint top = appData->jni->GetIntField(rect, topFieldID);
    jint bottom = appData->jni->GetIntField(rect, bottomFieldID);

    appData->jni->DeleteLocalRef(rectClass);
    appData->jni->DeleteLocalRef(rect);
    appData->jni->DeleteLocalRef(activityClass);

    jint innerHeight = bottom - top;
    jint innerWidth = right - left;

    float x = (float)innerWidth / 2.0f + (float)left; // + left _inset_
    float y = (float)innerHeight / 2.0f + (float)(ANativeWindow_getHeight(appData->app->window) - bottom); // + bottom _inset_

    return ff::Rectangle(x, y, (float)innerWidth, (float)innerHeight);
}
void handle_cmd(android_app* app, int32_t cmd) {
    AppData* appData = static_cast<AppData*>(app->userData);

    switch(cmd) {
        case APP_CMD_INIT_WINDOW: // android_app->window has a new window surface (ANativeWindow is ready for use).
            FF_CONSOLE_LOG("APP_CMD_INIT_WINDOW");
            {
                // A window is about to be shown, get it ready
                FF_ASSERT(appData->app->window, "App window was not provided.");
                setWindowToFullscreen(appData); // This will change the window size, do not query before calling this

                initDisplay(appData);

                // Update AndroidGameLoop with the updated back buffer dimensions
                {
                    int width = appData->surface.queryWidth();
                    int height = appData->surface.queryHeight();
                    static_cast<ff::AndroidGameLoop*>(&appData->gameServicer->getGameLoop())
                        ->setBackBufferDimensions(glm::ivec2(width, height));
                }

                if(!appData->initialized) {
                    appData->initialized = true;

                    FF_CONSOLE_LOG("Providing asset bundle...");
                    ff::Locator::provide(
                            new ff::AndroidAssetBundle(appData->app->activity->assetManager));

                    FF_CONSOLE_LOG("Providing backends...");
                    ff::provideBackends();

                    // Initialize
                    appData->gameServicer->init(appData->commandLineOptions);

                    // Get safe area
                    ff::Rectangle safeArea = getSafeArea(appData);
                    glm::vec2 windowBounds(ANativeWindow_getWidth(appData->app->window),
                       ANativeWindow_getHeight(appData->app->window));

                    // Compute safe area padding
                    float const topInset = windowBounds.y - safeArea.getTopRight().y;
                    float const bottomInset = safeArea.getBottomLeft().y;
                    float const paddingTop = topInset / windowBounds.y;
                    float const paddingBottom = bottomInset / windowBounds.y;
                    ff::CVars::get<float>("view_safe_area_padding_top") = paddingTop;
                    ff::CVars::get<float>("view_safe_area_padding_bottom") = paddingBottom;
                }
            }
            break;
        case APP_CMD_CONTENT_RECT_CHANGED:
            FF_CONSOLE_LOG("APP_CMD_CONTENT_RECT_CHANGED");
            // Can be used to get properties about the content rectangle.
            // For example, soft keyboard covering part of the window can
            // be reported.
            break;
        case APP_CMD_WINDOW_RESIZED:
            FF_CONSOLE_LOG("APP_CMD_WINDOW_RESIZED");
            // @todo THis would be used to detect resizes. What is the difference between this and rect changed?
            FF_CONSOLE_LOG("App window size: <%s, %s>",
               ANativeWindow_getWidth(appData->app->window),
               ANativeWindow_getHeight(appData->app->window));

            // @todo Does this go here or CONTENT_RECT_CHANGED?
            // @todo Does surface need to be updated when this event happens?

            // Update AndroidGameLoop with the updated back buffer dimensions
            {
                int width = appData->surface.queryWidth();
                int height = appData->surface.queryHeight();
                static_cast<ff::AndroidGameLoop*>(&appData->gameServicer->getGameLoop())
                        ->setBackBufferDimensions(glm::ivec2(width, height));
            }
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED: // System needs the window to be redrawn.
            FF_CONSOLE_LOG("APP_CMD_WINDOW_REDRAW_NEEDED");
            // @todo Change to actually redraw frame instead of just clearing.
            clearScreen(appData);
            swapBuffers(appData);
            break;
        case APP_CMD_TERM_WINDOW: // The existing ANativeWindow needs to be terminated.
            FF_CONSOLE_LOG("APP_CMD_TERM_WINDOW");
            termDisplay(appData, true);
            break;

        case APP_CMD_START: // Activity has been started.
            FF_CONSOLE_LOG("APP_CMD_START");
            break;
        case APP_CMD_GAINED_FOCUS: // Gained input focus
            FF_CONSOLE_LOG("APP_CMD_GAINED_FOCUS");
            if(appData->environment != nullptr) {
                appData->environment->startSensors();
            }
            break;
        case APP_CMD_LOST_FOCUS: // Lost input focus
            FF_CONSOLE_LOG("APP_CMD_LOST_FOCUS");
            ff::Locator::getMessageBus().dispatch<ff::EnvPauseRequestedEvent>();
            appData->environment->stopSensors();
            break;
        case APP_CMD_LOW_MEMORY: // System is running low on memory and has requested resources be reduced.
            FF_CONSOLE_LOG("APP_CMD_LOW_MEMORY");
            break;
        case APP_CMD_PAUSE: // App's activity has been paused (going into background)
            FF_CONSOLE_LOG("APP_CMD_PAUSE");
            // In ff, this is referred to "environment suspend".
            ff::Locator::getMessageBus().dispatch<ff::EnvPrepareForSuspendCommand>();
            break;
        case APP_CMD_RESUME: // App's activity has been resumed (coming from background)
            FF_CONSOLE_LOG("APP_CMD_RESUME");
            // In ff, this is referred to "environment restore".
            ff::Locator::getMessageBus().dispatch<ff::EnvPrepareForRestoreCommand>();
            break;
        case APP_CMD_STOP: // Activity has been stopped.
            FF_CONSOLE_LOG("APP_CMD_STOP");
            break;
        case APP_CMD_DESTROY: // Activity is being destroyed, clean up now
            FF_CONSOLE_LOG("APP_CMD_DESTROY");

            // @todo Handle shutdown.
            termDisplay(appData, false);
            break;
        default:
            break;
    }
}
int32_t handle_input(struct android_app* app, AInputEvent* event) {
    AppData* appData = static_cast<AppData*>(app->userData);

    int32_t eventType = AInputEvent_getType(event);
    switch(eventType) {
    case AINPUT_EVENT_TYPE_MOTION: {
        int32_t eventSource = AInputEvent_getSource(event);
        if((eventSource & AINPUT_SOURCE_TOUCHSCREEN) != 0
            && (eventSource & AINPUT_SOURCE_CLASS_POINTER) != 0) {
            if (appData->initialized) {
                int32_t action = AMotionEvent_getAction(event);
                int32_t actionCode = action & AMOTION_EVENT_ACTION_MASK;
                int32_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                    >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

                float x = AMotionEvent_getX(event, pointerIndex);
                float y = AMotionEvent_getY(event, pointerIndex);

                float alphaX = x / appData->width;
                float alphaY = y / appData->height;

                float normX = alphaX * 2 - 1;
                float normY = alphaY * 2 - 1;
                normY *= -1;

                switch (actionCode) {
                    case AMOTION_EVENT_ACTION_UP:
                    case AMOTION_EVENT_ACTION_CANCEL:
                        ff::Locator::getMessageBus().dispatch<ff::MouseButtonUpEvent>(
                                ff::MouseButton::PRIMARY, glm::vec2(normX, normY));
                        // Temporary solution to make touches seem more like a mouse
                        // (when touch ends, "mouse" does not move away and buttons look like
                        // they're still being hovered)
                        ff::Locator::getMessageBus().dispatch<ff::MouseMoveEvent>(
                            glm::vec2(std::numeric_limits<float>::max(),
                                      std::numeric_limits<float>::max()));
                        return 1;
                    case AMOTION_EVENT_ACTION_DOWN:
                        ff::Locator::getMessageBus().dispatch<ff::MouseMoveEvent>(glm::vec2(normX, normY));
                        ff::Locator::getMessageBus().dispatch<ff::MouseButtonDownEvent>(
                                ff::MouseButton::PRIMARY, glm::vec2(normX, normY));
                        return 1;
                    case AMOTION_EVENT_ACTION_MOVE:
                        ff::Locator::getMessageBus().dispatch<ff::MouseMoveEvent>(glm::vec2(normX, normY));
                        return 1;
                }
            }
        }
        break;
    }
    default:
        break;
    }
    return 0; // Return 0 for default dispatching, 1 if the event was handled (absorbed?)
}

#define FF_JNI_EXCEPTION_CHECK(jni) if((jni)->ExceptionCheck()) { /*jni->ExceptionDescribe();*/ FF_CONSOLE_FATAL("JNI had an exception."); }

void queryAudioManagerProperties(AppData* appData, int* sampleRate, int* framesPerBurst) {
    // If not wanting to use GetObjectClass method (or need another class in the package): https://dens.website/howto/use-jni-findclass-android

    jclass activityClass = appData->jni->GetObjectClass(appData->app->activity->clazz);

    jmethodID getAudioSampleRate = appData->jni->GetMethodID(activityClass, "getAudioSampleRate", "()I");
    jmethodID getAudioFramesPerBuffer = appData->jni->GetMethodID(activityClass, "getAudioFramesPerBuffer", "()I");

    *sampleRate = (int)appData->jni->CallIntMethod(appData->app->activity->clazz, getAudioSampleRate);
    *framesPerBurst = (int)appData->jni->CallIntMethod(appData->app->activity->clazz, getAudioFramesPerBuffer);

    appData->jni->DeleteLocalRef(activityClass);
}
std::vector<std::string> getCLIOptions(AppData* appData) {
    jclass activityClass = appData->jni->GetObjectClass(appData->app->activity->clazz);

    jmethodID getLaunchOptionsMethod = appData->jni->GetMethodID(activityClass, "getCLIOptions", "()[Ljava/lang/String;");
    jobjectArray optionsArray = (jobjectArray)appData->jni->CallObjectMethod(appData->app->activity->clazz, getLaunchOptionsMethod);

    int length = appData->jni->GetArrayLength(optionsArray);
    std::vector<std::string> options;
    options.reserve(length);
    for(int i = 0; i < length; i++) {
        jstring optionStrJ = (jstring)appData->jni->GetObjectArrayElement(optionsArray, i);
        char const* optionStr = appData->jni->GetStringUTFChars(optionStrJ, nullptr);
        options.push_back(optionStr);
        appData->jni->ReleaseStringUTFChars(optionStrJ, optionStr);
        appData->jni->DeleteLocalRef(optionStrJ);
    }

    appData->jni->DeleteLocalRef(optionsArray);
    appData->jni->DeleteLocalRef(activityClass);

    return options;
}

extern "C" void android_main(struct android_app* app) {
    // Source on native app glue: https://developer.android.com/reference/games/game-activity/group/android-native-app-glue

    // Register handlers
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;

    ff::Console::setConsoleInstance(new ff::LogcatConsole());
    FF_CONSOLE_LOG("`android_main` called.");

    // Set up userdata
    FF_CONSOLE_LOG("Set up userdata...");
    AppData* appData = static_cast<AppData*>(app->userData = new AppData{});
    appData->app = app;
    app->activity->vm->AttachCurrentThread(&appData->jni, nullptr);
    FF_ASSERT(appData->jni != nullptr, "Could not attach JNIEnv.");

    ff::GameServicer gameServicer(std::unique_ptr<ff::GameLoop>(new ff::AndroidGameLoop(app,
        appData->jni)));
    appData->gameServicer = &gameServicer;

    // Pre-initialization
    std::string configDir = tinyformat::format("%s/config", appData->app->activity->internalDataPath);

    ff::CommandLineOptions commandLineOptions(getCLIOptions(appData));
    appData->commandLineOptions = commandLineOptions;

    appData->environment = new ff::AndroidEnvironment(app,
        appData->jni,
        configDir);
    gameServicer.preInit(appData->commandLineOptions, appData->environment);

    // Pre-initialization means CVars have been initialized
    int oboeSampleRate, oboeFramesPerBurst;
    FF_CONSOLE_LOG("Querying audio manager properties...");
    queryAudioManagerProperties(appData, &oboeSampleRate, &oboeFramesPerBurst);

    ff::CVars::get<int>("audio_oboe_app_sample_rate") = oboeSampleRate;
    ff::CVars::get<int>("audio_oboe_app_frames_per_burst") = oboeSampleRate;

    // Full initialization has to happen after the window has been created.
    appData->initialized = false;

    float acculmulator = 0;

    // Main event loop
    int ident;
    int events;
    android_poll_source* source;
    while((appData->initialized && appData->gameServicer->getGame().getAlive())
        || (!appData->initialized && !app->destroyRequested)) {
        if ((ident = ALooper_pollAll(0, nullptr, &events, (void **) &source))
            >= 0) {
            if (source) {
                source->process(app, source);
            }

            if(appData->environment->isInitialized()) {
                appData->environment->processSensorEvents(ident);
            }
        }

        if(app->destroyRequested) {
            ff::Locator::getMessageBus().dispatch<ff::GameShutdownCommand>();
        }

        if(appData->display.good()
            && appData->surface.good()
            && appData->context.good()) {
            if(appData->initialized) {
                appData->gameServicer->service(acculmulator);
            } else {
                clearScreen(appData);
            }
            swapBuffers(appData);
        }
    }

    termDisplay(appData, false);
}
