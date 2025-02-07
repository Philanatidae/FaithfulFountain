/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SUPPORT_DESKTOP_ICLIPBOARD_PROVIDER_HPP
#define _FAITHFUL_FOUNTAIN_SUPPORT_DESKTOP_ICLIPBOARD_PROVIDER_HPP

#include <string>

namespace ff {

struct IClipboardProvider {
    virtual ~IClipboardProvider() = default;

    virtual std::string getClipboard() = 0;
};

class NullClipboardProvider final : public IClipboardProvider {
public:
    virtual ~NullClipboardProvider() = default;

    std::string getClipboard() override {
        return "";
    }
};

}

#endif
