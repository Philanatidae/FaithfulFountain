/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_PROCESSES_PROCESS_HPP
#define _FAITHFUL_FOUNTAIN_PROCESSES_PROCESS_HPP

#include <memory>
#include <cstdint>

#include <ff/processes/ProcessModifier.hpp>
#include <set>

#include <type_traits>

namespace ff {
    using ProcessPriority_t = uint64_t;

    namespace ProcessPriority {
        constexpr ProcessPriority_t INHERITED = (ProcessPriority_t)0;
        constexpr ProcessPriority_t EXTREMELY_LOW = (ProcessPriority_t)1;
        constexpr ProcessPriority_t VERY_LOW = (ProcessPriority_t)100;
        constexpr ProcessPriority_t LOW = (ProcessPriority_t)10000;
        constexpr ProcessPriority_t MEDIUM_LOW = (ProcessPriority_t)1000000;
        constexpr ProcessPriority_t MEDIUM = (ProcessPriority_t)100000000;
        constexpr ProcessPriority_t MEDIUM_HIGH = (ProcessPriority_t)10000000000;
        constexpr ProcessPriority_t HIGH = (ProcessPriority_t)1000000000000;
        constexpr ProcessPriority_t VERY_HIGH = (ProcessPriority_t)100000000000000;
        constexpr ProcessPriority_t EXTREMELY_HIGH = (ProcessPriority_t)10000000000000000;

        constexpr ProcessPriority_t DEFAULT = MEDIUM;
    }
    namespace ProcessFlags {
        constexpr int INITIALIZE_ON_ATTACH = 1 << 0;
        constexpr int REQUEST_NO_PAUSE = 1 << 1;
    }

    class Process : public std::enable_shared_from_this<Process> {
    friend class ProcessManager;

    public:
        Process(ProcessPriority_t const& priority = ProcessPriority::INHERITED);
        virtual ~Process();

        void preUpdate();
        void update(const float& dt);
        void postUpdate();
        void kill();
        void softKill();
        void togglePause();

        const std::shared_ptr<Process>& getNext() const;
        std::shared_ptr<Process> setNext(const std::shared_ptr<Process>& pNext);

        const bool& getInitialized() const;
        const bool& getAlive() const;
        const bool& getPaused() const;
        const bool& getSoftKillTriggered() const;

        ProcessPriority_t getPriority() const;

        Process* attachModifier(std::unique_ptr<ProcessModifier>&& modifier);
        template<typename T, typename... Args, typename std::enable_if<std::is_base_of<ProcessModifier, T>::value>::type* = nullptr>
        inline Process* attachModifier(Args... args) {
            return attachModifier(std::make_unique<T>(args...));
        }

        const std::set<std::unique_ptr<ProcessModifier>>& getModifiers();

        std::shared_ptr<Process> addFlags(const int& flag);
        bool hasFlag(const int& flag);

    protected:
        virtual void onInitialize();
        virtual void onPreUpdate();
        virtual void onUpdate(const float& dt);
        virtual void onPostUpdate();
        virtual void onKill();
        virtual void onSoftKill();
        virtual void onTogglePause();

        void setPriority(ProcessPriority_t const& priority);
    private:
        std::shared_ptr<Process> _pNext;

        bool _attached;
        bool _initialized;
        bool _alive;
        bool _paused;
        bool _softKillTriggered;

        ProcessPriority_t _priority;
        int _flags;

        std::set<std::unique_ptr<ProcessModifier>> _modifiers;

        void initialize();
        void initializeModifiers();
        void updateModifiers(const float& dt);
        void killModifiers();
        void togglePauseModifiers();
    };
}

#endif
