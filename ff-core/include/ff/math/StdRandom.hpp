/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MATH_STDRANDOM_HPP
#define _FAITHFUL_FOUNTAIN_MATH_STDRANDOM_HPP

#include <ff/math/IRandom.hpp>

#include <random>

namespace ff {
    class StdRandom : public IRandom {
    public:
        StdRandom();
        ~StdRandom();

        void setSeed(const unsigned int& seed) override;

        uint32_t next() override;

    protected:
        uint32_t getMax() const override;

    private:
        std::default_random_engine _generator;
        std::uniform_int_distribution<uint32_t> _distribution;
    };
}

#endif
