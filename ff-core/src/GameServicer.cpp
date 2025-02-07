/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/GameServicer.hpp>
#include <ff/CVars.hpp>
#include <ff/Console.hpp>
#include <ff/Locator.hpp>
#include <ff/processes/NullProcess.hpp>
#include <ff/assets/DirectoryAssetBundle.hpp>
#include <memory>
#include <stdlib.h>

#include <ff/audio/AudioCore.hpp>

#include <ff/graphics/RenderCore.hpp>
#include <ff/commands/render/RenderCmd.hpp>

#include <ff/directors/ClearConsoleDirector.hpp>

#include <timer_lib/timer.h>

#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <filesystem>

#if defined(FF_DEV_FEATURES)
#include <ff/dev/ImGuiCore.hpp>
#include <ff/dev/DevToolsCore.hpp>
#endif

#include <ff/dev/CommonCmdCore.hpp>

#if FF_HARD_LINK_ENTRY
FF_ENTRY_DECL void ff_entry(ff::Game* game);
#endif

namespace ff {
    GameLoop::GameLoop() {
        Locator::getMessageBus().addListener<GameShutdownEvent>(this);
    }
    GameLoop::~GameLoop() {
    }

    bool GameLoop::processEvent(const GameShutdownEvent& message) {
        FF_UNUSED(message);

        onShutdown();

        return false;
    }

    GameServicer::GameServicer(std::unique_ptr<GameLoop>&& gameLoopPtr)
        :_gameLoopPtr(std::move(gameLoopPtr)),
        _libraryHandle(nullptr),
        _preInitialized(false),
        _loopTimeInit(false),
        _forceZeroDelta(false) {
    }
    GameServicer::~GameServicer() {
#if !defined(FF_HARD_LINK_ENTRY)
        unloadGameLibrary();
#endif

        // Cleanup locator
        ff::Locator::cleanup();
    }

    GameLoop& GameServicer::getGameLoop() {
        return *_gameLoopPtr;
    }
    Game& GameServicer::getGame() {
        return _game;
    }

    const bool& GameServicer::hasPreInitialized() const {
        return _preInitialized;
    }

    bool GameServicer::processEvent(EnvPrepareForRestoreEvent const& evt) {
        _forceZeroDelta = true;

        return false;
    }
    bool GameServicer::processEvent(EnvPrepareForSuspendEvent const& evt) {
        _forceZeroDelta = false;

        return false;
    }

    void GameServicer::preInit(const CommandLineOptions& commandLineOptions, IEnvironment* const& environment) {
        FF_CONSOLE_LOG("Begin game pre-initialization...");

        // NOTE: `timer_lib_shutdown` currently doesn't do anything,
        // won't even bother calling it.
        FF_ASSERT(timer_lib_initialize() == 0, "Could not initialize `timer_lib`.")

        FF_CONSOLE_LOG("Initializing environment...");
        Locator::provide(environment);
        Locator::getEnvironment().init();

        FF_CONSOLE_LOG("Preparing engine CVars...");
        CVars::prepareCVars(commandLineOptions);

#if !defined(FF_HARD_LINK_ENTRY)
        FF_CONSOLE_LOG("Loading game library...");
        loadGameLibrary();
        // Since the game library likely bring in new CVars, we need to prepare them
        FF_CONSOLE_LOG("Preparing game CVars...");
        CVars::prepareCVars(commandLineOptions);
#endif

        FF_CONSOLE_LOG("Registering game servicer events...");
        FF_CONSOLE_WARN("Restore/suspend events need to be re-implemented.");
        //Locator::getMessageBus().addListener<EnvPrepareForRestoreCommand>(this);
        //Locator::getMessageBus().addListener<EnvPrepareForSuspendCommand>(this);

        FF_CONSOLE_LOG("Pre-initialization complete. Services can now be provided.");
        _preInitialized = true;
    }
    void GameServicer::init(const CommandLineOptions& commandLineOptions) {
#ifdef FF_DEV_FEATURES
        FF_CONSOLE_WARN("Development features are enabled");
#endif

        FF_CONSOLE_LOG("Begin game initialization...");

        FF_CONSOLE_LOG("Initializing asset bundle...");
        Locator::getAssetBundle().init();

        FF_CONSOLE_LOG("Attaching audio core...");
        _game.attachProcess(std::make_shared<AudioCore>());

        FF_CONSOLE_LOG("Attaching render core...");
        auto renderCore = std::make_shared<RenderCore>(_game.getProcessManager());
        _game.attachProcess(renderCore);

#if defined(FF_DEV_FEATURES)
        FF_CONSOLE_LOG("Attaching ImGui core...");
        _game.attachProcess(std::make_shared<ImGuiCore>());

        FF_CONSOLE_LOG("Attaching Dev Tools...");
        _game.attachProcess(std::make_shared<DevToolsCore>());
#endif
        _game.attachProcess(std::make_shared<CommonCmdCore>());
        _game.attachProcess(std::make_shared<ClearConsoleDirector>());

        FF_CONSOLE_LOG("Initializing game loop...");
        _gameLoopPtr->init(&_game);

        FF_CONSOLE_LOG("Obtaining back buffer dimensions...");
        glm::ivec2 backBufferDimensions = _gameLoopPtr->getBackBufferDimensions();

        // @todo Texture managers should be initialized AFTER the graphics
        //  device (this is a matter of completeness), but [at least Metal]
        //  the graphics device is using texture manager during initialization.
        FF_CONSOLE_LOG("Initializing color texture manager...");
        Locator::getColorTextureManager().initialize();
        FF_CONSOLE_LOG("Initializing depth texture manager...");
        Locator::getDepthTextureManager().initialize();

        FF_CONSOLE_LOG("Initializing graphics device...");
        Locator::getGraphicsDevice().init(&Locator::getAssetBundle());
        Locator::getGraphicsDevice().setBackBufferDimensions(backBufferDimensions);

        // Entry
        FF_CONSOLE_LOG("Preparing to run game entry...");
        runEntry();

        // If the game doesn't have any process (either a core or
        // the entry attaching its own processes), attach a null
        // process so the game doesn't close right away.
        if(!_game.getAlive()) {
            _game.attachProcess(std::make_shared<NullProcess>());
        }

        FF_CONSOLE_LOG("Game initialization complete.");
    }

    void GameServicer::loadGameLibrary() {
#if !defined(FF_HARD_LINK_ENTRY)
#if defined(FF_STATIC_GAME_ENTRY)
        FF_CONSOLE_LOG("Game library was statically linked to the platform executable.");
#endif

#if !defined(FF_STATIC_GAME_ENTRY)
        // Load library
        std::string const& libraryPathStr = CVars::get<std::string>("game_library_path");
        std::filesystem::path libraryPath = libraryPathStr;
        FF_ASSERT(std::filesystem::exists(libraryPath), "Game library does not exist at `%s`.", libraryPath.string());
#endif

#if defined(WIN32)
    #if defined(FF_STATIC_GAME_ENTRY)
        _libraryHandle = GetModuleHandle(nullptr);
    #else
        _libraryHandle = LoadLibrary(libraryPath.c_str());
    #endif
#else
    #if defined(FF_STATIC_GAME_ENTRY)
        _libraryHandle = dlopen(nullptr, RTLD_LAZY);
    #else
        _libraryHandle = dlopen(libraryPath.c_str(), RTLD_LAZY);
    #endif
#endif
        if (!_libraryHandle) {
#if defined(FF_STATIC_GAME_ENTRY)
            FF_CONSOLE_ERROR("Could not load game library (static).");
#else
            FF_CONSOLE_ERROR("Could not load game library at `%s`.", libraryPath.string());
#endif
            Locator::getMessageBus().dispatch<ff::ShutdownCmd>();
            return;
        }
        FF_CONSOLE_LOG("Game library loaded successfully.");
#endif
    }
    void GameServicer::unloadGameLibrary() {
#if defined(FF_HARD_LINK_ENTRY)
        // Close the library
        if(_libraryHandle != nullptr) {
#if defined(WIN32)
            FreeLibrary((HINSTANCE)_libraryHandle);
#else
            dlclose(_libraryHandle);
#endif
        }
#endif
    }
    void GameServicer::runEntry() {
#if !defined(FF_HARD_LINK_ENTRY)
        FF_ASSERT(_libraryHandle != nullptr, "Game library was not loaded.");

        // Load symbol
        typedef void (*ff_entry_t)(ff::Game*);

#if !defined(WIN32)
        // Reset errors
        dlerror();
#endif
#if defined(WIN32)
        ff_entry_t ff_entry = (ff_entry_t)GetProcAddress((HINSTANCE)_libraryHandle, "ff_entry");
#else
        ff_entry_t ff_entry = (ff_entry_t)dlsym(_libraryHandle, "ff_entry");
#endif

#if defined(WIN32)
        if (!ff_entry) {
            FF_CONSOLE_ERROR("Could not load symbol `ff_entry`.");
#else
        const char* dlsym_error = dlerror();
        if (dlsym_error) {
            FF_CONSOLE_ERROR("Could not load symbol `ff_entry`: %s", dlsym_error);
#endif
            Locator::getMessageBus().dispatch<ff::ShutdownCmd>();
            return;
        }
#endif
#if defined(FF_STATIC_GAME_ENTRY)
        FF_CONSOLE_LOG("`entry` was statically linked to ff-core.");
#endif
#if defined(FF_HARD_LINK_ENTRY)
        FF_CONSOLE_LOG("`entry` was hard linked to ff-core.");
#endif

        // Call function
        FF_CONSOLE_LOG("Running entry...");
        ff_entry(&_game);
    }

    void GameServicer::service(float& acculmulator) {
        // @todo Move acculmulator to GameService, after doing maintenance on FFBrickGame,
        //      there is no reason that the acculmulator needs to be external.

        _gameLoopPtr->pollEvents();

        if(!_loopTimeInit) {
            _prevLoopTicks = timer_current();
            _loopTimeInit = true;
        }
        // http://maniccoder.blogspot.com/2011/03/timing.html
        float const dt = _forceZeroDelta ? 0 : timer_elapsed(_prevLoopTicks);
        _prevLoopTicks = timer_current();

        Locator::getStatistics().pushListValue("Frame dt (ms)", dt);

        if(CVars::get<bool>("graphics_vsync")) {
            if(CVars::get<bool>("graphics_delta_smoothing")) {
                // Smoothing is only available when vsync is enabled, since
                // frames are always displayed at equal intervals. When vsync
                // is disabled, frames are shown ASAP, so the dt can be
                // measured simply by the time since the last frame.
                // 
                // Normally you can just hard-set the delta time to the
                // monitor's refresh rate, which works fine when the graphics
                // device can keep up with whatever you are doing. However,
                // if your frames suddly lower to refresh rate / 2, your
                // delta will no longer represent time between frames presented.
                // 
                // OpenGL does not report the time that a frame was presented,
                // so we have to do a smoothing approximation.
                // 
                // Metal _does_ report the time that the last frame was presented,
                // so we can use this information to do a better approximation.
                //
                // Additional reading: https://github.com/godotengine/godot/issues/30791
                // https://medium.com/@alen.ladavac/the-elusive-frame-timing-168f899aec92
                // http://bitsquid.blogspot.com/2010/10/time-step-smoothing.html
                // http://frankforce.com/?p=2636
                
                // @todo Replace with smoothing algorithm
                // I have no idea why this was hardcoded...
                //timeSinceLastFrame = 1/60.0f;
                // Smoothing algorithm needs to be changed, currently it assumes vsync is always running,
                // which isn't guaranteed to be the case. We need some sort of statistics on the frame
                // deltas to determine if vsync is enabled or not.
                // @todo Why is this hardcoded out???
                //smoothDeltaTime(&timeSinceLastFrame);
            }
        }

        acculmulator += dt;
        if(acculmulator > CVars::get<float>("acculmulator_max_before_reset")) {
            acculmulator = 0;
        }
        const float tickPeriod = 1 / CVars::get<float>("tick_frequency");
        while (acculmulator >= tickPeriod) {
            acculmulator -= tickPeriod;
            update(tickPeriod);
        }

        render(tickPeriod,
            acculmulator,
            dt);

        _gameLoopPtr->onService();
    }

    void GameServicer::update(const float& tickPeriod) {
        // @todo This needs to be wrapped with an @autoreleasepool
        // Or, ya know, just have the game loop do it itself...
        _gameLoopPtr->update(tickPeriod);
        _game.update(tickPeriod);
    }
    void GameServicer::render(const float& tickPeriod, const float& acculmulator, const float& timeSinceLastFrame) {
        // @todo This needs to be wrapped with an @autoreleasepool
        // This one actually does because Metal will do whatever it wants
        Locator::getGraphicsDevice().preRender();
        float betweenFrameAlpha;
        if (ff::CVars::get<bool>("graphics_frame_smoothing")) {
            betweenFrameAlpha = acculmulator / tickPeriod;
        }
        else {
            betweenFrameAlpha = 1; // Always render the latest transforms
        }

        Locator::getMessageBus().dispatch<RenderCmd>(betweenFrameAlpha, timeSinceLastFrame);

        Locator::getGraphicsDevice().postRender();
    }

    void GameServicer::smoothDeltaTime(float* const& dt) {
        if(*dt < 0.001f) {
            return;
        }

        // Ideally, if the graphics device supports reporting frame presentation
        // time (Metal, Vulkan), we should use that for a better calculation.
        // 
        // Currently this is a very naive implementation. Take the dt,
        // turn it into frame rate, round to multiple of refresh rate,
        // convert back into dt.
        // 
        // A game that struggles to hit the target frame rate will stutter,
        // unfortunately. This algorithm will definitely need to be worked
        // on in the future, hopefully the next project.
        // 
        // Vulkan presentation time write-up: http://themaister.net/blog/2018/11/09/experimenting-with-vk_google_display_timing-taking-control-over-the-swap-chain/
        // @todo Re-evalulate smoothing algorithm.
        float const frameRate = 1 / *dt;
        float const refreshRate = _gameLoopPtr->getTargetRefreshRate();
        float const multiple = frameRate / refreshRate;
        float const roundedMultiple = glm::round(multiple);
        float const newFrameRate = roundedMultiple * refreshRate;
        Locator::getStatistics().pushListValue("Smoothed Frame Rate (hz)", newFrameRate);
        *dt = 1 / newFrameRate;
    }
}
