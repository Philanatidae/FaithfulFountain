/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/io/ByteSerializer.hpp>

#include <glm/glm.hpp>
#include <numeric>
#include <vector>

#include <ff/Console.hpp>

namespace ff {
 
ByteSerializer::ByteSerializer(int const& bytesPerPage)
    :_bytesPerPage(bytesPerPage),
    _direction(SerializerDirection::WRITE),
    _head(0),
    _writtenBytes(0) {
    _pages.push_back(std::vector<uint8_t>());
    if(bytesPerPage > 0) {
        _pages[0].resize(_bytesPerPage, 0);
    }
}

SerializerDirection ByteSerializer::getDirection() const {
    return _direction;
}
void ByteSerializer::setDirection(SerializerDirection const& dir) {
    _direction = dir;
}

void ByteSerializer::endObject() {
}

void ByteSerializer::pushByte(uint8_t const& x) {
    FF_ASSERT(_direction == SerializerDirection::WRITE, "Serializer must be set to WRITE.");

    uint64_t pageHead = _head;
    int page = 0;
    if(_bytesPerPage > 0) { 
        pageHead %= _bytesPerPage;
        page = pageHead / _bytesPerPage;
    }
    _pages[page][pageHead] = x;
    _writtenBytes++;
    _head++;
    if(_bytesPerPage > 0) {
       if(_head / _bytesPerPage > _pages.size()) {
            _pages.push_back(std::vector<uint8_t>());
            _pages[_head / _bytesPerPage].resize(_bytesPerPage);
        }
    }
}
uint8_t ByteSerializer::popByte() {
    FF_ASSERT(_direction == SerializerDirection::READ, "Serializer must be set to READ.");

    uint64_t pageHead = _head;
    int page = 0;
    if(_bytesPerPage > 0) { 
        pageHead %= _bytesPerPage;
        page = pageHead / _bytesPerPage;
    }

    uint8_t x = _pages[page][pageHead];
    _head++;

    return x;
}
uint8_t ByteSerializer::peakByte() const {
    FF_ASSERT(_direction == SerializerDirection::READ, "Serializer must be set to READ.");
    FF_ASSERT(_head >= _writtenBytes, "Head out of range.");

    uint64_t pageHead = _head;
    int page = 0;
    if(_bytesPerPage > 0) { 
        pageHead %= _bytesPerPage;
        page = pageHead / _bytesPerPage;
    }

    return _pages[page][pageHead];
}

void ByteSerializer::resetHead() {
    _head = 0;
}
void ByteSerializer::setHeadToEnd() {
    _head = _writtenBytes - 1;
}
void ByteSerializer::seekForward(uint64_t const& amount) {
    _head += amount;
    _head = glm::min(_head, _writtenBytes - 1);
}
void ByteSerializer::seekBackward(uint64_t const& amount) {
    if(amount > _head) {
        _head = 0;
        return;
    }
    _head -= amount;
}

void ByteSerializer::serializeImpl(char const* const label,
    uint8_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        serialize("",
            exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            pushByte(x);
        } else {
            x = popByte();
        }
    }
}
void ByteSerializer::serializeImpl(char const* const label,
    uint16_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        serialize("",
            exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            uint8_t b1 = (uint8_t)(x & 0xFF);
            uint8_t b2 = (uint8_t)((x >> 8) & 0xFF);

            pushByte(b1);
            pushByte(b2);
        } else {
            uint8_t b1 = popByte();
            uint8_t b2 = popByte();

            x = 0;
            x |= b2;
            x <<= 8;
            x |= b1;
        }
    }
}
void ByteSerializer::serializeImpl(char const* const label,
    uint32_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        serialize("",
            exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            uint8_t b1 = (uint8_t)(x & 0xFF);
            uint8_t b2 = (uint8_t)((x >> 8) & 0xFF);
            uint8_t b3 = (uint8_t)((x >> 16) & 0xFF);
            uint8_t b4 = (uint8_t)((x >> 24) & 0xFF);

            pushByte(b1);
            pushByte(b2);
            pushByte(b3);
            pushByte(b4);
        } else {
            uint8_t b1 = popByte();
            uint8_t b2 = popByte();
            uint8_t b3 = popByte();
            uint8_t b4 = popByte();

            x = 0;
            x |= b4;
            x <<= 8;
            x |= b3;
            x <<= 8;
            x |= b2;
            x <<= 8;
            x |= b1;
        }
    }
}
void ByteSerializer::serializeImpl(char const* const label,
    uint64_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        serialize("",
            exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            uint8_t b1 = (uint8_t)(x & 0xFF);
            uint8_t b2 = (uint8_t)((x >> 8) & 0xFF);
            uint8_t b3 = (uint8_t)((x >> 16) & 0xFF);
            uint8_t b4 = (uint8_t)((x >> 24) & 0xFF);
            uint8_t b5 = (uint8_t)((x >> 32) & 0xFF);
            uint8_t b6 = (uint8_t)((x >> 40) & 0xFF);
            uint8_t b7 = (uint8_t)((x >> 48) & 0xFF);
            uint8_t b8 = (uint8_t)((x >> 56) & 0xFF);

            pushByte(b1);
            pushByte(b2);
            pushByte(b3);
            pushByte(b4);
            pushByte(b5);
            pushByte(b6);
            pushByte(b7);
            pushByte(b8);
        } else {
            uint8_t b1 = popByte();
            uint8_t b2 = popByte();
            uint8_t b3 = popByte();
            uint8_t b4 = popByte();
            uint8_t b5 = popByte();
            uint8_t b6 = popByte();
            uint8_t b7 = popByte();
            uint8_t b8 = popByte();

            x = 0;
            x |= b8;
            x <<= 8;
            x |= b7;
            x <<= 8;
            x |= b6;
            x <<= 8;
            x |= b5;
            x <<= 8;
            x |= b4;
            x <<= 8;
            x |= b3;
            x <<= 8;
            x |= b2;
            x <<= 8;
            x |= b1;
        }
    }
}

void ByteSerializer::serializeImpl(char const* const label,
    int8_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        serialize("",
            exists);
    }
    if(exists) {
        uint8_t cx = x;
        serialize(label, cx);
        if(_direction == SerializerDirection::READ) {
            x = (int8_t)cx;
        }
    }
}
void ByteSerializer::serializeImpl(char const* const label,
    int16_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        serialize("",
            exists);
    }
    if(exists) {
        uint16_t cx = x;
        serialize(label, cx);
        if(_direction == SerializerDirection::READ) {
            x = (int16_t)cx;
        }
    }
}
void ByteSerializer::serializeImpl(char const* const label,
    int32_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        serialize("",
            exists);
    }
    if(exists) {
        uint32_t cx = x;
        serialize(label, cx);
        if(_direction == SerializerDirection::READ) {
            x = (int32_t)cx;
        }
    }
}
void ByteSerializer::serializeImpl(char const* const label,
    int64_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        serialize("",
            exists);
    }
    if(exists) {
        uint64_t cx = x;
        serialize(label, cx);
        if(_direction == SerializerDirection::READ) {
            x = (int64_t)cx;
        }
    }
}

void ByteSerializer::serializeImpl(char const* const label,
    float& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        serialize("",
            exists);
    }
    if(exists) {
        // Only works if all compilers respect IEEE-754, which
        // is pretty much all of them at this point.
        uint32_t cx = *reinterpret_cast<uint32_t*>(&x);
        serialize(label, cx);
        if(_direction == SerializerDirection::READ) {
            x = *reinterpret_cast<float*>(&cx);
        }
    }
}
void ByteSerializer::serializeImpl(char const* const label,
    double& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        serialize("",
            exists);
    }
    if(exists) {
        // Only works if all compilers respect IEEE-754, which
        // is pretty much all of them at this point.
        uint64_t cx = *reinterpret_cast<uint64_t*>(&x);
        serialize(label, cx);
        if(_direction == SerializerDirection::READ) {
            x = *reinterpret_cast<double*>(&cx);
        }
    }
}

void ByteSerializer::serializeImpl(char const* const label,
    bool& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        serialize("",
            exists);
    }
    if(exists) {
        uint8_t cx = x ? 1 : 0;
        serialize(label, cx);
        if(_direction == SerializerDirection::READ) {
            x = cx != 0;
        }
    }
}

void ByteSerializer::serializeImpl(char const* const label,
    std::string& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        serialize("",
            exists);
    }
    if(exists) {
        // 16-bits for length of string, followed by string.
        FF_ASSERT(x.size() <= std::numeric_limits<uint16_t>::max(),
            "String exceeds maximum string length (%s).", std::numeric_limits<uint16_t>::max());


        if(_direction == SerializerDirection::WRITE) {
            uint16_t strLen = (uint16_t)x.size();

            uint8_t b1 = (uint8_t)(strLen & 0xFF);
            uint8_t b2 = (uint8_t)((strLen >> 8) & 0xFF);

            pushByte(b1);
            pushByte(b2);

            for(uint16_t i = 0; i < strLen; i++) {
                pushByte(x[i]);
            }
        } else {
            uint8_t b1 = popByte();
            uint8_t b2 = popByte();

            uint16_t strLen = 0;
            strLen = 0;
            strLen |= b2;
            strLen <<= 8;
            strLen |= b1;

            std::vector<char> chars;
            chars.reserve(strLen);
            for(uint16_t i = 0; i < strLen; i++) {
                chars.push_back(popByte());
            }

            x = std::string(chars.begin(), chars.end());
        }
    }
}

void ByteSerializer::beginObjectImpl(char const* const label,
    bool const& optional,
    bool& exists) {
    // @todo
}

}
