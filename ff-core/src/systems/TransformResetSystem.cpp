/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/systems/TransformResetSystem.hpp>

#include <ff/components/TransformComponent.hpp>

#include <ff/Locator.hpp>

namespace ff {
    TransformResetSystem::TransformResetSystem()
        :Process(ProcessPriority::EXTREMELY_LOW),
        _readyForTransformReset(true) {
    }

    void TransformResetSystem::onUpdate(const float& dt) {
        auto& actors = ff::Locator::getActorManager().getActorsFor(Family::all<TransformComponent>().get());
        actors.each([this](Actor_t actor) {
            ff::Locator::getActorManager().getComponent<TransformComponent>(actor).resetAll();
        });
    }
}
