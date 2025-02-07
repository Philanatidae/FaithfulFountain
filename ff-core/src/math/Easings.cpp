/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/math/Easings.hpp>

namespace ff {
    namespace Easings {
        float interpolate(const Functions& func, const float& p) {
            switch(func) {
                case Functions::QUADRATIC_IN:
                    return quadraticEaseIn(p);
                case Functions::QUADRATIC_OUT:
                    return quadraticEaseOut(p);
                case Functions::QUADRATIC_IN_OUT:
                    return quadraticEaseInOut(p);

                case Functions::CUBIC_IN:
                    return cubicEaseIn(p);
                case Functions::CUBIC_OUT:
                    return cubicEaseOut(p);
                case Functions::CUBIC_IN_OUT:
                    return cubicEaseInOut(p);

                case Functions::QUARTIC_IN:
                    return quarticEaseIn(p);
                case Functions::QUARTIC_OUT:
                    return quarticEaseOut(p);
                case Functions::QUARTIC_IN_OUT:
                    return quarticEaseInOut(p);

                case Functions::QUINTIC_IN:
                    return quinticEaseIn(p);
                case Functions::QUINTIC_OUT:
                    return quinticEaseOut(p);
                case Functions::QUINTIC_IN_OUT:
                    return quinticEaseInOut(p);

                case Functions::SINE_IN:
                    return sineEaseIn(p);
                case Functions::SINE_OUT:
                    return sineEaseOut(p);
                case Functions::SINE_IN_OUT:
                    return sineEaseInOut(p);

                case Functions::CIRCULAR_IN:
                    return circularEaseIn(p);
                case Functions::CIRCULAR_OUT:
                    return circularEaseOut(p);
                case Functions::CIRCULAR_IN_OUT:
                    return circularEaseInOut(p);

                case Functions::EXPONENTIAL_IN:
                    return exponentialEaseIn(p);
                case Functions::EXPONENTIAL_OUT:
                    return exponentialEaseOut(p);
                case Functions::EXPONENTIAL_IN_OUT:
                    return exponentialEaseInOut(p);

                case Functions::ELASTIC_IN:
                    return elasticEaseIn(p);
                case Functions::ELASTIC_OUT:
                    return elasticEaseOut(p);
                case Functions::ELASTIC_IN_OUT:
                    return elasticEaseInOut(p);

                case Functions::BACK_IN:
                    return backEaseIn(p);
                case Functions::BACK_OUT:
                    return backEaseOut(p);
                case Functions::BACK_IN_OUT:
                    return backEaseInOut(p);

                case Functions::BOUNCE_IN:
                    return bounceEaseIn(p);
                case Functions::BOUNCE_OUT:
                    return bounceEaseOut(p);
                case Functions::BOUNCE_IN_OUT:
                    return bounceEaseInOut(p);

                case Functions::LINEAR:
                default:
                    return linear(p);
            }
        }
    }
}
