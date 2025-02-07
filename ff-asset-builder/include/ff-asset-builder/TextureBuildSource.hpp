/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSET_BUILDER_TEXTURE_BUILD_SOURCE_HPP
#define _FAITHFUL_FOUNTAIN_ASSET_BUILDER_TEXTURE_BUILD_SOURCE_HPP

#include <ff-asset-builder/BuildSource.hpp>

#include <string>
#include <filesystem>

namespace ff {
    class TextureBuildSource : public BuildSource {
    public:
        TextureBuildSource(const std::string& name,
            const bool& isAlphaPremultiplied);
        virtual ~TextureBuildSource();

        std::string getType() const override;

        const std::string& getName() const;
        const bool& isAlphaPremultiplied() const;

    private:
        std::string _name;
        bool _isAlphaPremultiplied;
    };
}

#endif
