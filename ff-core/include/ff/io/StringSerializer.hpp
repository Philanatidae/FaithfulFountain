/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_IO_STRING_SERIALIZER_HPP
#define _FF_IO_STRING_SERIALIZER_HPP

#include <ff/io/Serializer.hpp>

#include <vector>
#include <iostream>
#include <sstream>

namespace ff {

class StringSerializer : public Serializer {
public:
    StringSerializer();
    virtual ~StringSerializer() = default;

    SerializerDirection getDirection() const override;
    void setDirection(SerializerDirection const& dir) override;

    void endObject() override;

    std::string getString() const;
    std::string getRemainderFromHead();
    void setString(std::string const& str);

    int getElementCount() const;

protected:
    void serializeImpl(char const* const label,
        uint8_t& x,
        bool const& optional,
        bool& exists) override;
    void serializeImpl(char const* const label,
        uint16_t& x,
        bool const& optional,
        bool& exists) override;
    void serializeImpl(char const* const label,
        uint32_t& x,
        bool const& optional,
        bool& exists) override;
    void serializeImpl(char const* const label,
        uint64_t& x,
        bool const& optional,
        bool& exists) override;

    void serializeImpl(char const* const label,
        int8_t& x,
        bool const& optional,
        bool& exists) override;
    void serializeImpl(char const* const label,
        int16_t& x,
        bool const& optional,
        bool& exists) override;
    void serializeImpl(char const* const label,
        int32_t& x,
        bool const& optional,
        bool& exists) override;
    void serializeImpl(char const* const label,
        int64_t& x,
        bool const& optional,
        bool& exists) override;

    void serializeImpl(char const* const label,
        float& x,
        bool const& optional,
        bool& exists) override;
    void serializeImpl(char const* const label,
        double& x,
        bool const& optional,
        bool& exists) override;

    void serializeImpl(char const* const label,
        bool& x,
        bool const& optional,
        bool& exists) override;

    void serializeImpl(char const* const label,
        std::string& x,
        bool const& optional,
        bool& exists) override;

    void beginObjectImpl(char const* const label,
        bool const& optional,
        bool& exists) override;

private:
    SerializerDirection _direction;

    std::stringbuf _strBuf;
    std::iostream _stream;
    int _elementCount;

    void handleExists(bool& exists);
};

}

#endif


