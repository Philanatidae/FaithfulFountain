/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/math/StdRandom.hpp>

#include <stdlib.h>
#include <limits>

#include <ff/util/Time.hpp>

namespace ff {
    StdRandom::StdRandom()
        :_distribution((std::numeric_limits<uint32_t>::min)(), (std::numeric_limits<uint32_t>::max)()) {
        setSeed((unsigned int)getCurrentTimestamp());
    }
    StdRandom::~StdRandom() {
    }

    void StdRandom::setSeed(const unsigned int& seed) {
        _generator.seed(seed);
    }
        
    uint32_t StdRandom::next() {
        return _distribution(_generator);
    }

    uint32_t StdRandom::getMax() const {
        return (std::numeric_limits<uint32_t>::max)();
    }
}
