/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MATH_IRANDOM_HPP
#define _FAITHFUL_FOUNTAIN_MATH_IRANDOM_HPP

#include <stdint.h>

namespace ff {
    class IRandom {
    public:
        IRandom();
        virtual ~IRandom();

        virtual void setSeed(const unsigned int& seed) = 0;

        virtual uint32_t next() = 0;
        float nextSingle();

        template<typename T>
        T next(const T& min, const T& max);

    protected:
        virtual uint32_t getMax() const = 0;
    };

    template<typename T>
    T IRandom::next(const T& min, const T& max) {
        if(min == max) {
            return min;
        }
        return (T)(((float)(max - min)) * nextSingle() + min);
    }
}

#endif
