/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/io/BinaryMemory.hpp>

#include <ff/Console.hpp>

namespace ff {
    BinaryMemory::BinaryMemory(uint8_t* const& ptr, const int& size) {
        _buffer.resize(size);
        if(ptr == nullptr) {
            std::memset(_buffer.data(), 0, size);
        } else {
            std::memcpy(_buffer.data(), ptr, size);
        }
    }
    BinaryMemory::BinaryMemory(const int& size)
        :BinaryMemory(nullptr, size) {
    }
    BinaryMemory::BinaryMemory(const std::vector<uint8_t>& _buffer)
        :_buffer(_buffer) {
    }
    BinaryMemory::BinaryMemory(IAssetBundle& assetBundle, const nlohmann::json& assetObject) {
        FF_ASSERT(!assetObject["path"].is_null(), "Missing `path` in asset object.");
        auto readerPtr = assetBundle.getAssetReader(assetObject["path"]);
        
        _buffer.resize(readerPtr->getSize());
        readerPtr->read(_buffer.data(), readerPtr->getSize());
    }
    BinaryMemory::BinaryMemory(BinaryReader& reader) {
        _buffer.resize(reader.getSize());
        reader.read(_buffer.data(), reader.getSize());
    }
    BinaryMemory::BinaryMemory(std::istream& stream) {
        std::streampos beg = stream.tellg();
        stream.seekg(0, std::ios_base::end);
        std::streampos end = stream.tellg();
        stream.seekg(0, std::ios_base::beg);
        std::streamsize size = end - beg;

        _buffer.reserve((size_t)size);
        _buffer.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
    }

    uint8_t* BinaryMemory::data() {
        return _buffer.data();
    }
    uint8_t const* BinaryMemory::data() const {
        return _buffer.data();
    }
    int BinaryMemory::size() const {
        return (int)_buffer.size();
    }

    std::string BinaryMemory::toString() const {
        return std::string(reinterpret_cast<char const*>(data()), size() * sizeof(char));
    }

    void BinaryMemory::copyFrom(void const* from, const int& size, const int& offset) {
        memcpy(&_buffer.data()[offset], from, size > -1 ? size : this->size());
    }
}
