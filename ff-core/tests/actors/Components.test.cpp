/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <catch2/catch_test_macros.hpp>

#include <ff/actors/ActorManager.hpp>

#include <memory>

using namespace ff;

struct Test1Component : Component<Test1Component> {
    Test1Component(const bool& b = false, const int& i = -1) : b(b), i(i) {}
    bool b;
    int i;
};

TEST_CASE("Components can be added to actors.", "[actors") {
    ActorManager actorManager;
    Actor_t actor = actorManager.createActor();
    SECTION("With full constructor.") {
        actorManager.addComponent<Test1Component>(actor, true, 10);
        REQUIRE(actorManager.getComponent<Test1Component>(actor).b == true);
        REQUIRE(actorManager.getComponent<Test1Component>(actor).i == 10);
    }
    SECTION("With partial constructor.") {
        actorManager.addComponent<Test1Component>(actor, true);
        REQUIRE(actorManager.getComponent<Test1Component>(actor).b == true);
        REQUIRE(actorManager.getComponent<Test1Component>(actor).i == -1);
    }
    SECTION("With empty constructor.") {
        actorManager.addComponent<Test1Component>(actor);
        REQUIRE(actorManager.getComponent<Test1Component>(actor).b == false);
        REQUIRE(actorManager.getComponent<Test1Component>(actor).i == -1);
    }
    REQUIRE(actorManager.hasComponent<Test1Component>(actor) == true);
}
TEST_CASE("Components can be removed from actors.", "[actors]") {
    ActorManager actorManager;
    Actor_t actor = actorManager.createActor();
    actorManager.addComponent<Test1Component>(actor);
    REQUIRE(actorManager.hasComponent<Test1Component>(actor) == true);
    actorManager.removeComponent<Test1Component>(actor);
    REQUIRE(actorManager.hasComponent<Test1Component>(actor) == false);
}

TEST_CASE("Components of the same type (different instances) can be added to different actors.", "[actors]") {
    ActorManager actorManager;
    Actor_t actor1 = actorManager.createActor();
    Actor_t actor2 = actorManager.createActor();
    actorManager.addComponent<Test1Component>(actor1);
    actorManager.addComponent<Test1Component>(actor2);
    REQUIRE(actorManager.hasComponent<Test1Component>(actor1) == true);
    REQUIRE(actorManager.hasComponent<Test1Component>(actor2) == true);
}

TEST_CASE("Component types are added to the component mask of the actor.", "[actors]") {
    ActorManager actorManager;
    Actor_t actor = actorManager.createActor();
    actorManager.addComponent<Test1Component>(actor);
    REQUIRE(actorManager.getComponentMask(actor) == Component<Test1Component>::getType());
}
TEST_CASE("Actor component masks are updated when a component is removed.", "[actors]") {
    ActorManager actorManager;
    Actor_t actor = actorManager.createActor();
    actorManager.addComponent<Test1Component>(actor);
    actorManager.removeComponent<Test1Component>(actor);
    REQUIRE(actorManager.getComponentMask(actor) == 0);
}

struct Test2Component : Component<Test2Component> {
    std::shared_ptr<int> i = nullptr;
};

TEST_CASE("Components are destructed when removed from an actor.", "[actors]") {
    ActorManager actorManager;
    Actor_t actor = actorManager.createActor();
    actorManager.addComponent<Test2Component>(actor);
    std::shared_ptr<int> iPtr = std::make_shared<int>(10);

    actorManager.getComponent<Test2Component>(actor).i = iPtr;
    REQUIRE(iPtr.use_count() == 2);
    actorManager.removeComponent<Test2Component>(actor);
    REQUIRE(iPtr.use_count() == 1);
}

TEST_CASE("Removing a component does not affect the memory location of other actors.", "[actors]") {
    ActorManager actorManager;
    Actor_t actor1 = actorManager.createActor();
    Actor_t actor2 = actorManager.createActor();
    actorManager.addComponent<Test1Component>(actor1);
    actorManager.addComponent<Test1Component>(actor2);
    SECTION("Remove component from actor 1.") {
        Test1Component* loc = &actorManager.getComponent<Test1Component>(actor2);
        actorManager.removeComponent<Test1Component>(actor1);
        REQUIRE(loc == &actorManager.getComponent<Test1Component>(actor2));
    }
    SECTION("Remove component from actor 2.") {
        Test1Component* loc = &actorManager.getComponent<Test1Component>(actor1);
        actorManager.removeComponent<Test1Component>(actor2);
        REQUIRE(loc == &actorManager.getComponent<Test1Component>(actor1));
    }
}

TEST_CASE("Components are unique per actor.", "[actors]") {
    ActorManager actorManager;
    Actor_t actor1 = actorManager.createActor();
    Actor_t actor2 = actorManager.createActor();
    actorManager.addComponent<Test1Component>(actor1);
    actorManager.addComponent<Test1Component>(actor2);

    REQUIRE(&actorManager.getComponent<Test1Component>(actor1)
        != &actorManager.getComponent<Test1Component>(actor2));
    
    REQUIRE(actorManager.getComponent<Test1Component>(actor1).b == false);
    REQUIRE(actorManager.getComponent<Test1Component>(actor2).b == false);

    actorManager.getComponent<Test1Component>(actor1).b = true;

    REQUIRE(actorManager.getComponent<Test1Component>(actor1).b == true);
    REQUIRE(actorManager.getComponent<Test1Component>(actor2).b == false);
}
