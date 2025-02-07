/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_IO_SERIALIZER_HPP
#define _FF_IO_SERIALIZER_HPP

#include <stdint.h>
#include <string>
#include <ff/Console.hpp>
#include <optional>

namespace ff {

template<typename T>
struct CustomSerializer {
};

enum class SerializerPrimitives {
    UNKNOWN = -1,

    UINT8 = 0,
    UINT16,
    UINT32,
    UINT64,

    INT8,
    INT16,
    INT32,
    INT64,

    FLOAT,
    DOUBLE,

    BOOL,

    STRING,

    OBJECT,

    COUNT
};
enum class SerializerDirection {
    READ,
    WRITE
};
class Serializer {
public:
    virtual ~Serializer() = default;

    virtual SerializerDirection getDirection() const = 0;
    virtual void setDirection(SerializerDirection const& dir) = 0;

    void serialize(char const* const label,
        uint8_t& x);
    void serialize(char const* const label,
        uint16_t& x);
    void serialize(char const* const label,
        uint32_t& x);
    void serialize(char const* const label,
        uint64_t& x);

    void serialize(char const* const label,
        int8_t& x);
    void serialize(char const* const label,
        int16_t& x);
    void serialize(char const* const label,
        int32_t& x);
    void serialize(char const* const label,
        int64_t& x);

    void serialize(char const* const label,
        float& x);
    void serialize(char const* const label,
        double& x);

    void serialize(char const* const label,
        bool& x);

    void serialize(char const* const label,
        std::string& x);

    void beginObject(char const* const label);
    virtual void endObject() = 0;

    template<typename T, typename std::enable_if_t<!std::is_enum<T>::value, int> = 0>
    void serialize(char const* const label, T& t);

    template<typename T>
    void serialize(char const* const label, T*& tPtr);
    template<typename T>
    void serialize(char const* const label, std::shared_ptr<T>& tPtr);

    template<typename T, typename std::enable_if_t<std::is_enum<T>::value, int> = 0>
    void serialize(char const* const label, T& x);

    template<typename T>
    void serialize(char const* const label, std::optional<T>& x);

protected:
    virtual void serializeImpl(char const* const label,
        uint8_t& x,
        bool const& optional,
        bool& exists) = 0;
    virtual void serializeImpl(char const* const label,
        uint16_t& x,
        bool const& optional,
        bool& exists) = 0;
    virtual void serializeImpl(char const* const label,
        uint32_t& x,
        bool const& optional,
        bool& exists) = 0;
    virtual void serializeImpl(char const* const label,
        uint64_t& x,
        bool const& optional,
        bool& exists) = 0;

    virtual void serializeImpl(char const* const label,
        int8_t& x,
        bool const& optional,
        bool& exists) = 0;
    virtual void serializeImpl(char const* const label,
        int16_t& x,
        bool const& optional,
        bool& exists) = 0;
    virtual void serializeImpl(char const* const label,
        int32_t& x,
        bool const& optional,
        bool& exists) = 0;
    virtual void serializeImpl(char const* const label,
        int64_t& x,
        bool const& optional,
        bool& exists) = 0;

    virtual void serializeImpl(char const* const label,
        float& x,
        bool const& optional,
        bool& exists) = 0;
    virtual void serializeImpl(char const* const label,
        double& x,
        bool const& optional,
        bool& exists) = 0;

    virtual void serializeImpl(char const* const label,
        bool& x,
        bool const& optional,
        bool& exists) = 0;

    virtual void serializeImpl(char const* const label,
        std::string& x,
        bool const& optional,
        bool& exists) = 0;

    virtual void beginObjectImpl(char const* const label,
        bool const& optional,
        bool& exists) = 0;
    
private:
};

template<typename T, typename std::enable_if_t<!std::is_enum<T>::value, int>>
void Serializer::serialize(char const* const label, T& t) {
    CustomSerializer<T>::serialize(this, label, t);
}
template<typename T>
void Serializer::serialize(char const* const label, T*& tPtr) {
    // @todo Remove pointer serialization. Serializers should not be
    // allowed to serialize pointers, since they don't exist past
    // program lifetime.
    FF_ASSERT(sizeof(tPtr) == 8, "Pointer is not 64-bits.");
    uint64_t cx = *reinterpret_cast<uint64_t*>(&tPtr);
    serialize(label, cx);
    if(getDirection() == SerializerDirection::READ) {
        tPtr = *reinterpret_cast<T**>(&cx);
    }
}
template<typename T>
void Serializer::serialize(char const* const label, std::shared_ptr<T>& tPtr) {
    // @todo Remove pointer serialization.
}

template<typename T, typename std::enable_if_t<std::is_enum<T>::value, int>>
void Serializer::serialize(char const* const label, T& x) {
    int cx = (int)x;
    serialize(label, cx);
    if(getDirection() == SerializerDirection::READ) {
        x = (T)cx;
    }
}

template<typename T>
void Serializer::serialize(char const* const label, std::optional<T>& x) {
    if(getDirection() == SerializerDirection::WRITE) {
        bool exists = x != std::nullopt;
        T xVal;
        if(exists) {
            xVal = x.value();
        }
        serializeImpl(label, xVal, true, exists);
    } else {
        bool exists;
        T xVal;
        serializeImpl(label, xVal, true, exists);
        if(exists) {
            x = xVal;
        }
    }
}

}

#endif

