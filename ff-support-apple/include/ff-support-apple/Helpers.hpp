/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SUPPORT_APPLE_HELPERS_HPP
#define _FAITHFUL_FOUNTAIN_SUPPORT_APPLE_HELPERS_HPP

#include <string>

namespace ff {
    namespace Helpers {
        bool isSandboxed();

        bool canOpenURL();
        void openURL(const std::string& url);
    }
}

#endif
