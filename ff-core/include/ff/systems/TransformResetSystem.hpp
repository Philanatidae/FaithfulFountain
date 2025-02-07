/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SYSTEMS_TRANSFORM_RESET_SEMI_SYSTEM_HPP
#define _FAITHFUL_FOUNTAIN_SYSTEMS_TRANSFORM_RESET_SEMI_SYSTEM_HPP

#include <ff/processes/Process.hpp>

namespace ff {
    class TransformResetSystem final : public Process {
    public:
        TransformResetSystem();
        ~TransformResetSystem() = default;

    protected:
        void onUpdate(const float& dt) override;

    private:
        bool _readyForTransformReset;
    };
}

#endif
