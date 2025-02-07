/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/CVars.hpp>

FF_CVAR_DEFINE(ios_motion_update_frequency, float, 60, ff::CVarFlags::PRESERVE, "Frequency at which motion data is polled from CMMotionManager.")
