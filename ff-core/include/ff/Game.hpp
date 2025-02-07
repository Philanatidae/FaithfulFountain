/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GAME_HPP
#define _FAITHFUL_FOUNTAIN_GAME_HPP

#include <ff/util/Time.hpp>
#include <ff/processes/ProcessManager.hpp>
#include <ff/messages/MessageBus.hpp>
#include <ff/messages/CmdHandler.hpp>
#include <ff/commands/ShutdownCmd.hpp>

#if defined(WIN32)
    #define FF_ENTRY_DECL extern "C" __declspec(dllexport)
#else
    #if defined(__APPLE__)
        #define FF_ENTRY_DECL extern "C"
    #else
        //#define FF_ENTRY_DECL __attribute__ ((visibility("default"))) extern "C" Supposed to work but doesn't for some odd reason. Need to test on Linux (eventually) to see if it is a Linux thing or an Android thing
        #define FF_ENTRY_DECL extern "C"
    #endif
#endif

namespace ff {
    class Game : public CmdHandler<ShutdownCmd> {
    public:
        Game();
        virtual ~Game();

        void update(const float& dt);

        bool getAlive() const;

        std::shared_ptr<Process> attachProcess(std::shared_ptr<Process> process);
        ProcessManager* getProcessManager() const;

        std::unique_ptr<typename ShutdownCmd::Ret> handleCmd(ShutdownCmd const& cmd) override;

    private:
        ProcessManager _processManager;

        void shutdown();
    };
}

#endif
