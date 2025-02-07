/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/io/Serializer.hpp>

namespace ff {

void Serializer::serialize(char const* const label,
    uint8_t& x) {
    bool exists = true;
    serializeImpl(label, x, false, exists);
}
void Serializer::serialize(char const* const label,
    uint16_t& x) {
    bool exists = true;
    serializeImpl(label, x, false, exists);
}
void Serializer::serialize(char const* const label,
    uint32_t& x) {
    bool exists = true;
    serializeImpl(label, x, false, exists);
}
void Serializer::serialize(char const* const label,
    uint64_t& x) {
    bool exists = true;
    serializeImpl(label, x, false, exists);
}

void Serializer::serialize(char const* const label,
    int8_t& x) {
    bool exists = true;
    serializeImpl(label, x, false, exists);
}
void Serializer::serialize(char const* const label,
    int16_t& x) {
    bool exists = true;
    serializeImpl(label, x, false, exists);
}
void Serializer::serialize(char const* const label,
    int32_t& x) {
    bool exists = true;
    serializeImpl(label, x, false, exists);
}
void Serializer::serialize(char const* const label,
    int64_t& x) {
    bool exists = true;
    serializeImpl(label, x, false, exists);
}

void Serializer::serialize(char const* const label,
    float& x) {
    bool exists = true;
    serializeImpl(label, x, false, exists);
}
void Serializer::serialize(char const* const label,
    double& x) {
    bool exists = true;
    serializeImpl(label, x, false, exists);
}

void Serializer::serialize(char const* const label,
    bool& x) {
    bool exists = true;
    serializeImpl(label, x, false, exists);
}

void Serializer::serialize(char const* const label,
    std::string& x) {
    bool exists = true;
    serializeImpl(label, x, false, exists);
}

void Serializer::beginObject(char const* const label) {
    bool exists = true;
    beginObjectImpl(label, false, exists);
}

}

