/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ACTORS_ACTOR_DESTRUCTION_PROCESS_HPP
#define _FAITHFUL_FOUNTAIN_ACTORS_ACTOR_DESTRUCTION_PROCESS_HPP

#include <ff/processes/InstantProcess.hpp>
#include <ff/actors/Actor.hpp>
#include <ff/actors/ActorManager.hpp>

namespace ff {
    class ActorDestructionProcess final : public InstantProcess {
    public:
        ActorDestructionProcess(ActorManager* const& actorManagerPtr, const Actor_t& actor);
        ~ActorDestructionProcess() = default;

    protected:
        void onTrigger() override;

    private:
        ActorManager* const _actorManagerPtr;
        Actor_t _actor;
    };
}

#endif
