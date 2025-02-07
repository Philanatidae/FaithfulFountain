/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/Game.hpp>

#include <ff/processes/RenderProcess.hpp>

#include <ff/Locator.hpp>

#include <ff/events/GameShutdownEvent.hpp>

namespace ff {
    Game::Game() {
        Locator::getMessageBus().addHandler<ShutdownCmd>(this);
    }
    Game::~Game() {
    }

    void Game::update(const float& dt) {
        Locator::getStatistics().beginStopwatch("game_update_time");
        _processManager.tick(dt);
        Locator::getStatistics().pushListValue("Game Update Time (ms)", Locator::getStatistics().endStopwatch("game_update_time") * 1000);
    }

    bool Game::getAlive() const {
        return _processManager.getProcesses().size() > 0;
    }
    void Game::shutdown() {
        for (auto it = _processManager.getProcesses().begin();
            it != _processManager.getProcesses().end();
            it++) {
            (*it)->setNext(nullptr);
        }
        _processManager.killAll();
    }

    std::shared_ptr<Process> Game::attachProcess(std::shared_ptr<Process> process) {
        return _processManager.attachProcess(process);
    }
    ProcessManager* Game::getProcessManager() const {
        return const_cast<ProcessManager*>(&_processManager);
    }

    std::unique_ptr<typename ShutdownCmd::Ret> Game::handleCmd(ShutdownCmd const& cmd) {
        Locator::getMessageBus().dispatch<GameShutdownEvent>();
        shutdown();

        return std::make_unique<typename ShutdownCmd::Ret>();
    }
}
