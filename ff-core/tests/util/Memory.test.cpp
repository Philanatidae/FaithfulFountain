/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <catch2/catch_test_macros.hpp>

#include <ff/util/Memory.hpp>

class PointerTestClass {
public:
    PointerTestClass(bool& b)
        :_b(b) {
        _b = false;
    }
    ~PointerTestClass() {
        _b = true;
    }

    void changeBool(bool newB) {
        _b = newB;
    }

private:
    bool& _b;
};


//////
// UNIQUE_PTR
//////
TEST_CASE("unique_ptr deletes pointer when unique_ptr goes out of scope.", "[util]") {
    bool pointerDeleted = true;
    {
        ff::unique_ptr<PointerTestClass> ptr(new PointerTestClass(pointerDeleted));
    }
    REQUIRE(pointerDeleted);
}

TEST_CASE("unique_ptr can be used like a normal pointer.", "[util]") {
    ff::unique_ptr<int> ptr(new int(10));
    REQUIRE(*ptr == 10);

    REQUIRE(ptr);

    ff::unique_ptr<int> empty_ptr;
    REQUIRE(!empty_ptr);
}

TEST_CASE("unique_ptr can be reset and deletes the pointer.", "[util]") {
    ff::unique_ptr<int> ptr(new int(10));
    REQUIRE(ptr);
    ptr.reset();
    REQUIRE(!ptr);
}

TEST_CASE("unique_ptr can release its pointer without deleting.", "[util]") {
    ff::unique_ptr<int> ptr(new int(10));
    REQUIRE(ptr);
    int* rawPtr = ptr.release();
    REQUIRE(!ptr);
    REQUIRE(*rawPtr == 10);
    delete rawPtr;
}

TEST_CASE("unique_ptr can be moved to another unique_ptr.", "[util]") {
    ff::unique_ptr<int> ptr(new int(10));
    REQUIRE(ptr);
    REQUIRE(*ptr == 10);
    ff::unique_ptr<int> ptr2(std::move(ptr));
    REQUIRE(!ptr);
    REQUIRE(ptr2);
    REQUIRE(*ptr2 == 10);
}

TEST_CASE("unique_ptr can use arrow operator.", "[util]") {
    bool fnCalled = false;
    ff::unique_ptr<PointerTestClass> ptr(new PointerTestClass(fnCalled));
    REQUIRE(!fnCalled);
    ptr->changeBool(true);
    REQUIRE(fnCalled);
}

TEST_CASE("unique_ptr can be created with make_unique.", "[util]") {
    auto ptr = ff::make_unique<int>(10);
    REQUIRE(ptr);
    REQUIRE(*ptr == 10);
}

TEST_CASE("unique_ptr can have a custom deleter function.", "[util]") {
    bool deleterCalled = false;
    {
        auto ptr = ff::unique_ptr<int>(new int(10), [&deleterCalled](int* ptr) {
            delete ptr;
            deleterCalled = true;
        });
        REQUIRE(ptr);
    }
    REQUIRE(deleterCalled);
}

//////
// SHARED_PTR
//////
TEST_CASE("shared_ptr deletes pointer when shared_ptr goes out of scope.", "[util]") {
    bool pointerDeleted = true;
    {
        ff::shared_ptr<PointerTestClass> ptr(new PointerTestClass(pointerDeleted));
    }
    REQUIRE(pointerDeleted);
}

TEST_CASE("shared_ptr can be used like a normal pointer.", "[util]") {
    ff::shared_ptr<int> ptr(new int(10));
    REQUIRE(*ptr == 10);

    REQUIRE(ptr);

    ff::shared_ptr<int> empty_ptr;
    REQUIRE(!empty_ptr);
}

TEST_CASE("shared_ptr can be reset and deletes the pointer.", "[util]") {
    ff::shared_ptr<int> ptr(new int(10));
    REQUIRE(ptr);
    ptr.reset();
    REQUIRE(!ptr);
}

TEST_CASE("shared_ptr can be copied to another shared_ptr.", "[util]") {
    ff::shared_ptr<int> ptr(new int(10));
    REQUIRE(ptr);
    REQUIRE(*ptr == 10);
    ff::shared_ptr<int> ptr2 = ptr;
    REQUIRE(ptr);
    REQUIRE(ptr2);
    REQUIRE(*ptr2 == 10);
    REQUIRE(ptr.use_count() == 2);
}

TEST_CASE("shared_ptr can use arrow operator.", "[util]") {
    bool fnCalled = false;
    ff::shared_ptr<PointerTestClass> ptr(new PointerTestClass(fnCalled));
    REQUIRE(!fnCalled);
    ptr->changeBool(true);
    REQUIRE(fnCalled);
}

TEST_CASE("shared_ptr can be created with make_shared.", "[util]") {
    auto ptr = ff::make_shared<int>(10);
    REQUIRE(ptr);
    REQUIRE(*ptr == 10);
}

TEST_CASE("shared_ptr can have a custom deleter function.", "[util]") {
    bool deleterCalled = false;
    {
        auto ptr = ff::shared_ptr<int>(new int(10), [&deleterCalled](int* ptr) {
            delete ptr;
            deleterCalled = true;
        });
        REQUIRE(ptr);
    }
    REQUIRE(deleterCalled);
}

TEST_CASE("shared_ptr ref count works as it does in the standard library.", "[util]") {
    ff::shared_ptr<int> ptr;
    {
        ff::shared_ptr<int> ptr2 = ff::make_shared<int>(2);
        REQUIRE(ptr2.use_count() == 1);
        ptr = ptr2;
        REQUIRE(ptr.use_count() == 2);
        REQUIRE(ptr2.use_count() == 2);
    }
    REQUIRE(ptr.use_count() == 1);
}

TEST_CASE("shared_ptr can be set to nullptr.", "[util]") {
    ff::shared_ptr<int> ptr = ff::make_shared<int>(10);
    REQUIRE(ptr);
    REQUIRE(*ptr == 10);
    ptr = nullptr;
    REQUIRE(!ptr);
}

//////
// WEAK_PTR
//////
TEST_CASE("weak_ptr is only alive if shared_ptr is alive.", "[util]") {
    ff::weak_ptr<int> weakPtr;
    {
        ff::shared_ptr<int> ptr = ff::make_shared<int>(10);
        REQUIRE(ptr.use_count() == 1);
        weakPtr = ptr;
        REQUIRE(ptr.use_count() == 1);
        REQUIRE(!weakPtr.expired());
    }
    REQUIRE(weakPtr.expired());
}
TEST_CASE("weak_ptr can lock to generate a shared pointer if it is valid.", "[util]") {
    ff::weak_ptr<int> weakPtr;
    REQUIRE(weakPtr.lock() == nullptr);

    ff::shared_ptr<int> sharedPtr = ff::make_shared<int>(10);
    weakPtr = sharedPtr;
    auto weakPtrLock = weakPtr.lock();
    REQUIRE(weakPtrLock);
    REQUIRE(*weakPtrLock == 10);
    REQUIRE(sharedPtr.use_count() == 2);
}
