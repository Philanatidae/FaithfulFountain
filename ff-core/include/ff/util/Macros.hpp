/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_UTIL_MACROS_HPP
#define _FAITHFUL_FOUNTAIN_UTIL_MACROS_HPP

#include <cstddef>

#define FF_UNUSED(expr) do { (void)(expr); } while(0)

#define FF_OFFSET_OF(str, memb) ((int)offsetof(str, memb))

#define FF_TYPE_NAME(T) #T

#define FF_ADD_BITWISE_OPERATORS_TO_ENUM(enm) \
    inline constexpr enm operator|(const enm& lhs, const enm& rhs) {\
        return static_cast<enm>(static_cast<std::underlying_type<enm>::type>(lhs)\
            | static_cast<std::underlying_type<enm>::type>(rhs));\
    }\
    inline constexpr enm operator&(const enm& lhs, const enm& rhs) {\
        return static_cast<enm>(static_cast<std::underlying_type<enm>::type>(lhs)\
            & static_cast<std::underlying_type<enm>::type>(rhs));\
    }\
    inline constexpr enm operator^(const enm& lhs, const enm& rhs) {\
        return static_cast<enm>(static_cast<std::underlying_type<enm>::type>(lhs)\
            ^ static_cast<std::underlying_type<enm>::type>(rhs));\
    }\
    inline constexpr enm operator~(const enm& val) {\
        return static_cast<enm>(~static_cast<std::underlying_type<enm>::type>(val));\
    }\
    inline constexpr enm operator|=(const enm& lhs, const enm& rhs) {\
        return static_cast<enm>(static_cast<std::underlying_type<enm>::type>(lhs)\
            | static_cast<std::underlying_type<enm>::type>(rhs));\
    }\
    inline constexpr enm operator&=(const enm& lhs, const enm& rhs) {\
        return static_cast<enm>(static_cast<std::underlying_type<enm>::type>(lhs)\
            & static_cast<std::underlying_type<enm>::type>(rhs));\
    }\
    inline constexpr enm operator^=(const enm& lhs, const enm& rhs) {\
        return static_cast<enm>(static_cast<std::underlying_type<enm>::type>(lhs)\
            ^ static_cast<std::underlying_type<enm>::type>(rhs));\
    }

#endif
