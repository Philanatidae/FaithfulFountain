/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_IO_BYTE_SERIALIZER_HPP
#define _FF_IO_BYTE_SERIALIZER_HPP

#include <ff/io/Serializer.hpp>

#include <vector>

namespace ff {

class ByteSerializer : public Serializer {
public:
    ByteSerializer(int const& bytesPerPage = -1);
    virtual ~ByteSerializer() = default;

    SerializerDirection getDirection() const override;
    void setDirection(SerializerDirection const& dir) override;

    void endObject() override;

    void pushByte(uint8_t const& x);
    uint8_t popByte();
    uint8_t peakByte() const;

    void resetHead();
    void setHeadToEnd();
    void seekForward(uint64_t const& amount);
    void seekBackward(uint64_t const& amount);

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
    uint32_t const _bytesPerPage;

    SerializerDirection _direction;

    uint64_t _head;
    uint64_t _writtenBytes;
    std::vector<std::vector<uint8_t>> _pages;
};

}

#endif

