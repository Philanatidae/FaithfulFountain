/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <catch2/catch_test_macros.hpp>

#include <ff/resources/ResourceHandle.hpp>

using namespace ff;

TEST_CASE("ResourceHandle can be created with a load function.", "[resources]") {
    ResourceHandle<int> handle = ResourceHandle<int>::createResource([]() -> int* {
        return new int(10);
    });
    REQUIRE(*handle == 10);
}
TEST_CASE("ResourceHandle can be created with a helper nullptr provider.", "[resources]") {
    ResourceHandle<int> handle = ResourceHandle<int>::createNullResource();
    REQUIRE(handle.get() == nullptr);
}
