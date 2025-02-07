/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MATH_EASINGS_HPP
#define _FAITHFUL_FOUNTAIN_MATH_EASINGS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace ff {
    namespace Easings {
        enum class Functions {
            LINEAR,

            QUADRATIC_IN,
            QUADRATIC_OUT,
            QUADRATIC_IN_OUT,

            CUBIC_IN,
            CUBIC_OUT,
            CUBIC_IN_OUT,

            QUARTIC_IN,
            QUARTIC_OUT,
            QUARTIC_IN_OUT,

            QUINTIC_IN,
            QUINTIC_OUT,
            QUINTIC_IN_OUT,

            SINE_IN,
            SINE_OUT,
            SINE_IN_OUT,

            CIRCULAR_IN,
            CIRCULAR_OUT,
            CIRCULAR_IN_OUT,

            EXPONENTIAL_IN,
            EXPONENTIAL_OUT,
            EXPONENTIAL_IN_OUT,

            ELASTIC_IN,
            ELASTIC_OUT,
            ELASTIC_IN_OUT,

            BACK_IN,
            BACK_OUT,
            BACK_IN_OUT,

            BOUNCE_IN,
            BOUNCE_OUT,
            BOUNCE_IN_OUT
        };

        inline float linear(const float& p) {
            return p;
        }

        inline float quadraticEaseIn(const float& p) {
            return p * p;
        }
        inline float quadraticEaseOut(const float& p) {
            return -(p * (p - 2));
        }
        inline float quadraticEaseInOut(const float& p) {
            if(p < 0.5f) {
                return 2 * p * p;
            } else {
                return (-2 * p * p) + (4 * p) - 1; 
            }
        }

        inline float cubicEaseIn(const float& p) {
            return p * p * p;
        }
        inline float cubicEaseOut(const float& p) {
            float f = (p - 1);
            return f * f * f + 1;
        }
        inline float cubicEaseInOut(const float& p) {
            if (p < 0.5f)
            {
                return 4 * p * p * p;
            }
            else
            {
                float f = ((2 * p) - 2);
                return 0.5f * f * f * f + 1;
            }
        }

        inline float quarticEaseIn(const float& p) {
            return p * p * p * p;
        }
        inline float quarticEaseOut(const float& p) {
            float f = (p - 1);
            return f * f * f * (1 - p) + 1;
        }
        inline float quarticEaseInOut(const float& p) {
            if (p < 0.5f)
            {
                return 8 * p * p * p * p;
            }
            else
            {
                float f = (p - 1);
                return -8 * f * f * f * f + 1;
            }
        }

        inline float quinticEaseIn(const float& p) {
            return p * p * p * p * p;
        }
        inline float quinticEaseOut(const float& p) {
            float f = (p - 1);
            return f * f * f * f * f + 1;
        }
        inline float quinticEaseInOut(const float& p) {
            if (p < 0.5f)
            {
                return 16 * p * p * p * p * p;
            }
            else
            {
                float f = ((2 * p) - 2);
                return 0.5f * f * f * f * f * f + 1;
            }
        }

        inline float sineEaseIn(const float& p) {
            return glm::sin((p - 1) * glm::half_pi<float>()) + 1;
        }
        inline float sineEaseOut(const float& p) {
            return glm::sin(p * glm::half_pi<float>());
        }
        inline float sineEaseInOut(const float& p) {
            return 0.5f * (1 - glm::cos(p * glm::pi<float>()));
        }

        inline float circularEaseIn(const float& p) {
            return 1 - glm::sqrt(1 - (p * p));
        }
        inline float circularEaseOut(const float& p) {
            return glm::sqrt((2 - p) * p);
        }
        inline float circularEaseInOut(const float& p) {
            if (p < 0.5f)
            {
                return 0.5f * (1 - glm::sqrt(1 - 4 * (p * p)));
            }
            else
            {
                return 0.5f * (glm::sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
            }
        }

        inline float exponentialEaseIn(const float& p) {
            return (float)glm::pow(2, 10 * (p - 1));
        }
        inline float exponentialEaseOut(const float& p) {
            return 1 - (float)glm::pow(2, -10 * p);
        }
        inline float exponentialEaseInOut(const float& p) {
            if (p < 0.5f)
            {
                return 0.5f * (float)glm::pow(2, (20 * p) - 10);
            }
            else
            {
                return -0.5f * (float)glm::pow(2, (-20 * p) + 10) + 1;
            }
        }

        inline float elasticEaseIn(const float& p) {
            return glm::sin(13 * glm::half_pi<float>() * p) * (float)glm::pow(2, 10 * (p - 1));
        }
        inline float elasticEaseOut(const float& p) {
            return glm::sin(-13 * glm::half_pi<float>() * (p + 1)) * (float)glm::pow(2, -10 * p) + 1;
        }
        inline float elasticEaseInOut(const float& p) {
            if (p < 0.5f)
            {
                return 0.5f * glm::sin(13 * glm::half_pi<float>() * (2 * p)) * (float)glm::pow(2, 10 * ((2 * p) - 1));
            }
            else
            {
                return 0.5f * (glm::sin(-13 * glm::half_pi<float>() * ((2 * p - 1) + 1)) * (float)glm::pow(2, -10 * (2 * p - 1)) + 2);
            }
        }

        inline float backEaseIn(const float& p) {
            return p * p * p - p * glm::sin(p * glm::pi<float>());
        }
        inline float backEaseOut(const float& p) {
            float f = (1 - p);
            return 1 - (f * f * f - f * glm::sin(f * glm::pi<float>()));
        }
        inline float backEaseInOut(const float& p) {
            if (p < 0.5f)
            {
                float f = 2 * p;
                return 0.5f * (f * f * f - f * glm::sin(f * glm::pi<float>()));
            }
            else
            {
                float f = (1 - (2 * p - 1));
                return 0.5f * (1 - (f * f * f - f * glm::sin(f * glm::pi<float>()))) + 0.5f;
            }
        }

        inline float bounceEaseOut(const float& p) {
            if (p < 4 / 11.0f)
            {
                return (121 * p * p) / 16.0f;
            }
            else if (p < 8 / 11.0f)
            {
                return (363 / 40.0f * p * p) - (99 / 10.0f * p) + 17 / 5.0f;
            }
            else if (p < 9 / 10.0f)
            {
                return (4356 / 361.0f * p * p) - (35442 / 1805.0f * p) + 16061 / 1805.0f;
            }
            else
            {
                return (54 / 5.0f * p * p) - (513 / 25.0f * p) + 268 / 25.0f;
            }
        }
        inline float bounceEaseIn(const float& p) {
            return 1 - bounceEaseOut(1 - p);
        }
        inline float bounceEaseInOut(const float& p) {
            if (p < 0.5f)
            {
                return 0.5f * bounceEaseIn(p * 2);
            }
            else
            {
                return 0.5f * bounceEaseOut(p * 2 - 1) + 0.5f;
            }
        }

        float interpolate(const Functions& func, const float& p);
    }
}

#endif
