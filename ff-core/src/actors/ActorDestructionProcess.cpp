/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/actors/ActorDestructionProcess.hpp>

namespace ff {
    ActorDestructionProcess::ActorDestructionProcess(ActorManager* const& actorManagerPtr, const Actor_t& actor)
        :_actorManagerPtr(actorManagerPtr),
        _actor(actor) {
    }

    void ActorDestructionProcess::onTrigger() {
        if(_actorManagerPtr->isActorAlive(_actor)) {
            _actorManagerPtr->destroyActor(_actor);
        }
    }
}
