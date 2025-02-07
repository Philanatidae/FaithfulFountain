/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MATH_HELPER_HPP
#define _FAITHFUL_FOUNTAIN_MATH_HELPER_HPP

#include <glm/glm.hpp>

namespace ff {
    namespace MathHelper {
        // Source: https://www.includehelp.com/cpp-programs/find-next-and-previous-power-of-two-of-a-given-number.aspx#:~:text=We%20will%20use%20Bitwise%20AND%20%28%20%26%20%29,number%20having%20only%20its%20left%20most%20bit%20set.
        template<typename T> // Constrain this to non-floating types
        inline T nextPowerOfTwo(const T& n) {
            T val = 1;
            while(val <= n) {
                val = val << 1;
            }
            return val;
        }
        template<typename T> // Constrain this to non-floating types
        inline T previousPowerOfTwo(T n) {
            if((n && !(n & (n - 1))) == 1) {
                return n >> 1;
            }
            while(n & n - 1) {
                n = n & n - 1;
            }
            return n;
        }
        
        template<typename T> // Constrain this to non-floating types
        inline bool isPowerOfTwo(T n) {
            return (n & (n - 1)) == 0;
        }

        inline bool isWithinEpsilon(const float& val, const float& target, const float& epsilon) {
            return glm::abs(val - target) < epsilon;
        }
        inline bool isWithinEpsilon(const double& val, const double& target, const double& epsilon) {
            return glm::abs(val - target) < epsilon;
        }

        template<typename T>
        inline T moveTowardsTickDependent(T const& from, T const& to, float const& factor) {
            // Note: This is tick-dependent.
            // See: https://www.reddit.com/r/gamedev/comments/cayb4f/basic_smooth_spring_movement/etc6ei6/
            // This function DOES NOT WORK when in a shader (fragment, specifically), because
            // we cannot guarantee what framerate the game will run at (FF does not allow
            // framerate locking).
            return glm::mix(from, to, factor);
        };
    }
}

#endif
