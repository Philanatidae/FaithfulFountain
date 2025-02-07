/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_UTIL_HASH_HPP
#define _FAITHFUL_FOUNTAIN_UTIL_HASH_HPP

#include <utility>

namespace ff {
    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v) {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    inline void hash_combine_num(uint64_t& seed, const uint64_t& v) {
        seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    // Note that since computers are limited in how large they can store
    // a number, if a or b are too large, it can overflow the 64-bit number.
    // Try to use as small values as possible for a and b. This function
    // accepts uint64_t as paramters for convenience.
    inline uint64_t cantor_pairing(const uint64_t& a, const uint64_t& b) {
        return (a + b) * (a + b + 1) / 2 + b;
    }
}

#endif
