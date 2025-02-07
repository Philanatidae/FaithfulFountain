/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/common/Reloadable.hpp>

namespace ff {
    Reloadable::Reloadable()
        :_isValid(true) {
    }
    Reloadable::~Reloadable() {
    }

    void Reloadable::reload() {
        if(_isValid) {
            invalidate();
        }
        onReload();
        _isValid = true;
    }
    void Reloadable::invalidate() {
        onInvalidate();
        _isValid = false;
    }

    const bool& Reloadable::getIsValid() const {
        return _isValid;
    }
}
