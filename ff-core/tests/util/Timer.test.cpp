/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <ff/util/Timer.hpp>

TEST_CASE("Timer is elapsed after the specified duration has passed", "[util]") {
    ff::Timer timer(10);
    timer.update(11);
    REQUIRE(timer.hasElapsed());
    REQUIRE(timer.getElapsed() == Catch::Approx(11.0));
}

TEST_CASE("Timer resets with the same duration.", "[util]") {
    ff::Timer timer(10);
    timer.update(11);
    timer.reset();
    REQUIRE(timer.getDuration() == Catch::Approx(10));
}

TEST_CASE("Timer can be reset with a different duration.", "[util]") {
    ff::Timer timer(10);
    timer.update(11);
    timer.reset(5);
    REQUIRE(timer.getDuration() == Catch::Approx(5));
}

TEST_CASE("Timer can return normalized progress.", "[util]") {
    ff::Timer timer(2);

    REQUIRE(timer.getAlpha() == Catch::Approx(0));
    REQUIRE(timer.getClampedAlpha() == Catch::Approx(0));

    timer.update(1);
    REQUIRE(timer.getAlpha() == Catch::Approx(0.5));
    REQUIRE(timer.getClampedAlpha() == Catch::Approx(0.5));

    timer.update(1);
    REQUIRE(timer.getAlpha() == Catch::Approx(1));
    REQUIRE(timer.getClampedAlpha() == Catch::Approx(1));

    timer.update(2);
    REQUIRE(timer.getAlpha() == Catch::Approx(2));
    REQUIRE(timer.getClampedAlpha() == Catch::Approx(1));
}
