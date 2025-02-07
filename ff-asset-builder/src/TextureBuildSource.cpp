/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/TextureBuildSource.hpp>

namespace ff {
    TextureBuildSource::TextureBuildSource(const std::string& name,
            const bool& isAlphaPremultiplied)
        :_name(name),_isAlphaPremultiplied(isAlphaPremultiplied) {
    }
    TextureBuildSource::~TextureBuildSource() {
    }

    std::string TextureBuildSource::getType() const {
        return "Texture";
    }

    const std::string& TextureBuildSource::getName() const {
        return _name;
    }
    const bool& TextureBuildSource::isAlphaPremultiplied() const {
        return _isAlphaPremultiplied;
    }
}
