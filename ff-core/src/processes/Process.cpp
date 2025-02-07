/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/processes/Process.hpp>

#include <ff/util/Macros.hpp>
#include <ff/Console.hpp>

namespace ff {
    Process::Process(ProcessPriority_t const& priority)
        :_attached(false),_initialized(false),_alive(true),_paused(false),
        _softKillTriggered(false),_priority(priority),_flags(0) {
    }
    Process::~Process() {
    }

    void Process::preUpdate() {
        if(!_initialized) {
            initialize();
        }

        if(_alive && !_paused) {
            onPreUpdate();
        }
    }
    void Process::update(const float& dt) {
        if(_alive && !_paused) {
            onUpdate(dt);
            updateModifiers(dt);
        }
    }
    void Process::postUpdate() {
        if(_alive && !_paused) {
            onPostUpdate();
        }
    }
    void Process::kill() {
        onKill();
        killModifiers();
        _alive = false;
    }
    void Process::softKill() {
        _softKillTriggered = true;
        onSoftKill();
    }
    void Process::togglePause() {
        _paused = !_paused;
        onTogglePause();
        togglePauseModifiers();
    }

    const std::shared_ptr<Process>& Process::getNext() const {
        return _pNext;
    }
    std::shared_ptr<Process> Process::setNext(const std::shared_ptr<Process>& pNext) {
        _pNext = pNext;
        if (_pNext == nullptr) {
            return nullptr;
        }
        if(_pNext->getPriority() <= ProcessPriority::INHERITED) {
            _pNext->setPriority(getPriority());
        }
        return _pNext;
    }

    const bool& Process::getInitialized() const {
        return _initialized;
    }
    const bool& Process::getAlive() const {
        return _alive;
    }
    const bool& Process::getPaused() const {
        return _paused;
    }
    const bool& Process::getSoftKillTriggered() const {
        return _softKillTriggered;
    }

    ProcessPriority_t Process::getPriority() const {
        return _priority;
    }

    Process* Process::attachModifier(std::unique_ptr<ProcessModifier>&& modifier) {
        modifier->attach(this);
        _modifiers.emplace(std::move(modifier));

        return this;
    }

    const std::set<std::unique_ptr<ProcessModifier>>& Process::getModifiers() {
        return _modifiers;
    }

    std::shared_ptr<Process> Process::addFlags(const int& flag) {
        if(_attached
            && flag != ProcessFlags::REQUEST_NO_PAUSE) {
            FF_CONSOLE_WARN("Flags are being added to the process after it has been attached (flags `%s`).", flag);
        }

        _flags |= flag;

        return this->shared_from_this();
    }
    bool Process::hasFlag(const int& flag) {
        return (_flags & flag) != 0;
    }

    void Process::setPriority(ProcessPriority_t const& priority) {
        _priority = priority;
    }

    void Process::onInitialize() {
    }
    void Process::onPreUpdate() {
    }
    void Process::onUpdate(const float& dt) {
        FF_UNUSED(dt);
    }
    void Process::onPostUpdate() {
    }
    void Process::onKill() {
    }
    void Process::onSoftKill() {
        kill();
    }
    void Process::onTogglePause() {
    }

    void Process::initialize() {
        initializeModifiers();
        onInitialize();
        _initialized = true;
    }
    void Process::initializeModifiers() {
        for(auto it = _modifiers.begin();
            it != _modifiers.end();
            it++) {
            (*it)->onInitialize();
        }
    }
    void Process::updateModifiers(const float& dt) {
        for(auto it = _modifiers.begin();
            it != _modifiers.end();
            it++) {
            (*it)->onUpdate(dt);
        }
    }
    void Process::killModifiers() {
        for(auto it = _modifiers.begin();
            it != _modifiers.end();
            it++) {
            (*it)->onKill();
        }
    }
    void Process::togglePauseModifiers() {
        for(auto it = _modifiers.begin();
            it != _modifiers.end();
            it++) {
            (*it)->onTogglePause();
        }
    }
}
