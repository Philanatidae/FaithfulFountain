/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_UTIL_CONJUNCTION_HPP
#define _FAITHFUL_FOUNTAIN_UTIL_CONJUNCTION_HPP

#include <type_traits>

namespace ff {

// Source: https://stackoverflow.com/a/36933392

template<typename...>
struct conjunction : std::true_type {
};

template<typename T>
struct conjunction<T> : T {
};

template<typename T, typename... Ts>
struct conjunction<T, Ts...>
    : std::conditional_t<T::value != false, conjunction<Ts...>, T>
{
};

}

#endif
