/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ENV_PREPARE_FOR_SUSPEND_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_ENV_PREPARE_FOR_SUSPEND_EVENT_HPP

#include <ff/messages/Event.hpp>

namespace ff {
    FF_EVENT_DEFINE_0(EnvPrepareForSuspendEvent,
        "evt_env_prepare_for_suspend");
}

#endif
