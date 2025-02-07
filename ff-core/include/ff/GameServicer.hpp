/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GAME_SERVICER_HPP
#define _FAITHFUL_FOUNTAIN_GAME_SERVICER_HPP

#include <ff/Game.hpp>
#include <ff/CommandLineOptions.hpp>
#include <ff/env/IEnvironment.hpp>
#include <ff/messages/EventListener.hpp>
#include <ff/events/GameShutdownEvent.hpp>
#include <ff/events/env/EnvPrepareForSuspendEvent.hpp>
#include <ff/events/env/EnvPrepareForRestoreEvent.hpp>
#include <functional>

#include <timer_lib/timer.h>

namespace ff {
    // @todo Change game loop to "runners"; the idea is that a "runner"
    // will call _gameServicer->service, instead of GameLoop. GameLoop
    // functionality can be moved into environment, or abstracted away
    // (might be a good time to implement multiple windows)

    class GameLoop : public EventListener<GameShutdownEvent> {
    public:
        GameLoop();
        virtual ~GameLoop();

        bool processEvent(const GameShutdownEvent& message) override;

        virtual void init(Game* const& gamePtr) = 0;
        virtual void update(const float& dt) = 0;
        virtual void pollEvents() = 0;
        virtual void onService() = 0;
        virtual void onShutdown() = 0;
        virtual float getTargetRefreshRate() = 0;
        virtual glm::ivec2 getBackBufferDimensions() = 0;
    };

    class NullGameLoop : public GameLoop {
    public:
        void init(Game* const& gamePtr) override {
        }
        void update(const float& dt) override {
        }
        void pollEvents() override {
        }
        void onService() override {
        }
        void onShutdown() override {
        }
        float getTargetRefreshRate() override {
            return -1;
        }
        glm::ivec2 getBackBufferDimensions() override {
            return glm::ivec2(-1, -1);
        }
    };

    class IGraphicsDevice;

    class GameServicer final
        : public EventListener<EnvPrepareForRestoreEvent>,
        public EventListener<EnvPrepareForSuspendEvent> {
    public:
        GameServicer(std::unique_ptr<GameLoop>&& gameLoopPtr);
        ~GameServicer();

        void preInit(const CommandLineOptions& commandLineOptions, IEnvironment* const& environment);
        void init(const CommandLineOptions& commandLineOptions);
        void service(float& acculmulator);

        GameLoop& getGameLoop();
        Game& getGame();

        const bool& hasPreInitialized() const;

        bool processEvent(EnvPrepareForRestoreEvent const& evt) override;
        bool processEvent(EnvPrepareForSuspendEvent const& evt) override;

    private:
        void loadGameLibrary();
        void unloadGameLibrary();
        void runEntry();

        void update(const float& tickPeriod);
        void render(const float& tickPeriod, const float& acculmulator, const float& timeSinceLastFrame);

        void smoothDeltaTime(float* const& dt);
        
        Game _game;
        std::unique_ptr<GameLoop> _gameLoopPtr;
        void* _libraryHandle;
        bool _preInitialized;

        tick_t _prevLoopTicks;
        bool _loopTimeInit;
        bool _forceZeroDelta;
    };
}

#endif
