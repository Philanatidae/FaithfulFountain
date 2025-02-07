/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_BINARY_MEMORY_HPP
#define _FAITHFUL_FOUNTAIN_BINARY_MEMORY_HPP

#include <vector>
#include <stdint.h>
#include <ff/assets/IAssetBundle.hpp>

namespace ff {
    class BinaryMemory {
    public:
        BinaryMemory(uint8_t* const& ptr, const int& size);
        BinaryMemory(const int& size);
        BinaryMemory(const std::vector<uint8_t>& _buffer);
        BinaryMemory(IAssetBundle& assetBundle, const nlohmann::json& assetObject);
        BinaryMemory(BinaryReader& reader);
        BinaryMemory(std::istream& stream);

        uint8_t* data();
        uint8_t const* data() const;
        int size() const;

        std::string toString() const;

        void copyFrom(void const* from, const int& size = -1, const int& offset = 0);

    private:
        std::vector<uint8_t> _buffer;
    };
}

#endif
