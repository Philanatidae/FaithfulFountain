/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_DIRECTORS_PRESERVE_CVARS_DIRECTOR_HPP
#define _FAITHFUL_FOUNTAIN_DIRECTORS_PRESERVE_CVARS_DIRECTOR_HPP

#include <ff/messages/CmdHandler.hpp>
#include <ff/processes/Process.hpp>
#include <ff/commands/PreserveCVarsCmd.hpp>

namespace ff {
    class PreserveCVarsDirector final : public Process,
        public CmdHandler<PreserveCVarsCmd> {
    public:
        PreserveCVarsDirector();
        ~PreserveCVarsDirector() = default;

        std::unique_ptr<typename PreserveCVarsCmd::Ret> handleCmd(PreserveCVarsCmd const& cmd) override ;

    protected:
        void onInitialize() override;
    };
}

#endif
