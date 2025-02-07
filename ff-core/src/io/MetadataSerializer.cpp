/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/io/MetadataSerializer.hpp>

namespace ff {

SerializerDirection MetadataSerializer::getDirection() const {
    return SerializerDirection::WRITE;
}
void MetadataSerializer::setDirection(SerializerDirection const& dir) {
    FF_ASSERT(dir == SerializerDirection::WRITE,
        "MetadataSerializer can only be WRITE.");
}

void MetadataSerializer::endObject() {
    // @todo Properly implement objects
}

void MetadataSerializer::serializeImpl(char const* const label,
    uint8_t& x,
    bool const& optional,
    bool& exists) {
    _args.emplace_back(label,
        SerializerPrimitives::UINT8,
        optional);
}
void MetadataSerializer::serializeImpl(char const* const label,
    uint16_t& x,
    bool const& optional,
    bool& exists) {
    _args.emplace_back(label,
        SerializerPrimitives::UINT16,
        optional);
}
void MetadataSerializer::serializeImpl(char const* const label,
    uint32_t& x,
    bool const& optional,
    bool& exists) {
    _args.emplace_back(label,
        SerializerPrimitives::UINT32,
        optional);
}
void MetadataSerializer::serializeImpl(char const* const label,
    uint64_t& x,
    bool const& optional,
    bool& exists) {
    _args.emplace_back(label,
        SerializerPrimitives::UINT64,
        optional);
}

void MetadataSerializer::serializeImpl(char const* const label,
    int8_t& x,
    bool const& optional,
    bool& exists) {
    _args.emplace_back(label,
        SerializerPrimitives::INT8,
        optional);
}
void MetadataSerializer::serializeImpl(char const* const label,
    int16_t& x,
    bool const& optional,
    bool& exists) {
    _args.emplace_back(label,
        SerializerPrimitives::INT16,
        optional);
}
void MetadataSerializer::serializeImpl(char const* const label,
    int32_t& x,
    bool const& optional,
    bool& exists) {
    _args.emplace_back(label,
        SerializerPrimitives::INT32,
        optional);
}
void MetadataSerializer::serializeImpl(char const* const label,
    int64_t& x,
    bool const& optional,
    bool& exists) {
    _args.emplace_back(label,
        SerializerPrimitives::INT64,
        optional);
}

void MetadataSerializer::serializeImpl(char const* const label,
    float& x,
    bool const& optional,
    bool& exists) {
    _args.emplace_back(label,
        SerializerPrimitives::FLOAT,
        optional);
}
void MetadataSerializer::serializeImpl(char const* const label,
    double& x,
    bool const& optional,
    bool& exists) {
    _args.emplace_back(label,
        SerializerPrimitives::DOUBLE,
        optional);
}

void MetadataSerializer::serializeImpl(char const* const label,
    bool& x,
    bool const& optional,
    bool& exists) {
    _args.emplace_back(label,
        SerializerPrimitives::BOOL,
        optional);
}

void MetadataSerializer::serializeImpl(char const* const label,
    std::string& x,
    bool const& optional,
    bool& exists) {
    _args.emplace_back(label,
        SerializerPrimitives::STRING,
        optional);
}

void MetadataSerializer::beginObjectImpl(char const* const label,
    bool const& optional,
    bool& exists) {
    // @todo Properly implement objects
}

std::vector<SerializerArgInfo> MetadataSerializer::getArgs() const {
    return _args;
}
void MetadataSerializer::clear() {
    _args.clear();
}

}
