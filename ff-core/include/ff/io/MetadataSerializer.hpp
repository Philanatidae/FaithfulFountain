/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_IO_METADATA_SERIALIZER_HPP
#define _FF_IO_METADATA_SERIALIZER_HPP

#include <ff/io/Serializer.hpp>
#include <string>
#include <vector>

namespace ff {

struct SerializerArgInfo {
    std::string name;
    SerializerPrimitives type;
    bool optional;

    SerializerArgInfo(std::string const& name,
        SerializerPrimitives const& type,
        bool const& optional)
        :name(name),
        type(type),
        optional(optional) {
    }
};
class MetadataSerializer : public Serializer {
public:
    virtual ~MetadataSerializer() = default;

    SerializerDirection getDirection() const override;
    void setDirection(SerializerDirection const& dir) override;

    void endObject() override;

    std::vector<SerializerArgInfo> getArgs() const;
    void clear();

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
    std::vector<SerializerArgInfo> _args;
};

}

#endif

