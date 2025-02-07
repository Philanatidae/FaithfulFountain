/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <catch2/catch_test_macros.hpp>

#include <ff/actors/ActorManager.hpp>
#include <ff/actors/Family.hpp>

using namespace ff;

struct Test1Component : Component<Test1Component> {
};
struct Test2Component : Component<Test2Component> {
};
struct Test3Component : Component<Test3Component> {
};

TEST_CASE("Families can be created that can test against an actor's component mask.", "[actor]") {
    ActorManager actorManager;
    Actor_t actor = actorManager.createActor();
    actorManager.addComponent<Test1Component>(actor);
    actorManager.addComponent<Test2Component>(actor);
    actorManager.addComponent<Test3Component>(actor);
    SECTION("A family can be created that matches all components.") {
        Family family(Family::all<Test1Component, Test2Component, Test3Component>().get());
        REQUIRE(family.matches(&actorManager, actor) == true);
        actorManager.removeComponent<Test1Component>(actor);
        REQUIRE(family.matches(&actorManager, actor) == false);
    }
    SECTION("A family can be created that matches any component.") {
        Family family(Family::one<Test1Component, Test2Component, Test3Component>().get());
        REQUIRE(family.matches(&actorManager, actor) == true);
        actorManager.removeComponent<Test1Component>(actor);
        REQUIRE(family.matches(&actorManager, actor) == true);
        actorManager.removeComponent<Test2Component>(actor);
        REQUIRE(family.matches(&actorManager, actor) == true);
        actorManager.removeComponent<Test3Component>(actor);
        REQUIRE(family.matches(&actorManager, actor) == false);
    }
    SECTION("A family can be created that matches none of the components.") {
        Family family(Family::exclude<Test1Component, Test2Component, Test3Component>().get());
        REQUIRE(family.matches(&actorManager, actor) == false);
        actorManager.removeComponent<Test1Component>(actor);
        REQUIRE(family.matches(&actorManager, actor) == false);
        actorManager.removeComponent<Test2Component>(actor);
        REQUIRE(family.matches(&actorManager, actor) == false);
        actorManager.removeComponent<Test3Component>(actor);
        REQUIRE(family.matches(&actorManager, actor) == true);
    }

    SECTION("A family can be created that combines filter types.") {
        Family family(Family::all<Test1Component, Test2Component>().exclude<Test3Component>().get());
        REQUIRE(family.matches(&actorManager, actor) == false);
        actorManager.removeComponent<Test3Component>(actor);
        REQUIRE(family.matches(&actorManager, actor) == true);
    }
}
TEST_CASE("ActorManager can return actors that match a family..", "[actor]") {
    ActorManager actorManager;
    Actor_t actor = actorManager.createActor();
    actorManager.addComponent<Test1Component>(actor);
    actorManager.addComponent<Test2Component>(actor);
    actorManager.addComponent<Test3Component>(actor);
    SECTION("A family can be created that matches all components.") {
        Family family(Family::all<Test1Component, Test2Component, Test3Component>().get());
        REQUIRE(actorManager.getActorsFor(family).count() == 1);
        actorManager.removeComponent<Test1Component>(actor);
        REQUIRE(actorManager.getActorsFor(family).count() == 0);
    }
    SECTION("A family can be created that matches any component.") {
        Family family(Family::one<Test1Component, Test2Component, Test3Component>().get());
        REQUIRE(actorManager.getActorsFor(family).count() == 1);
        actorManager.removeComponent<Test1Component>(actor);
        REQUIRE(actorManager.getActorsFor(family).count() == 1);
        actorManager.removeComponent<Test2Component>(actor);
        REQUIRE(actorManager.getActorsFor(family).count() == 1);
        actorManager.removeComponent<Test3Component>(actor);
        REQUIRE(actorManager.getActorsFor(family).count() == 0);
    }
    SECTION("A family can be created that matches none of the components.") {
        Family family(Family::exclude<Test1Component, Test2Component, Test3Component>().get());
        REQUIRE(actorManager.getActorsFor(family).count() == 0);
        actorManager.removeComponent<Test1Component>(actor);
        REQUIRE(actorManager.getActorsFor(family).count() == 0);
        actorManager.removeComponent<Test2Component>(actor);
        REQUIRE(actorManager.getActorsFor(family).count() == 0);
        actorManager.removeComponent<Test3Component>(actor);
        REQUIRE(actorManager.getActorsFor(family).count() == 1);
    }

    SECTION("A family can be created that combines filter types.") {
        Family family(Family::all<Test1Component, Test2Component>().exclude<Test3Component>().get());
        REQUIRE(actorManager.getActorsFor(family).count() == 0);
        actorManager.removeComponent<Test3Component>(actor);
        REQUIRE(actorManager.getActorsFor(family).count() == 1);
    }
}
TEST_CASE("Creating an actor that matches a family will be added to the family.", "[actor]") {
    ActorManager manager;
    Family family = Family::exclude<Test1Component>().get();
    REQUIRE(manager.getActorsFor(family).count() == 0);
    Actor_t actor = manager.createActor();
    REQUIRE(manager.getActorsFor(family).count() == 1);
    manager.addComponent<Test1Component>(actor);
    REQUIRE(manager.getActorsFor(family).count() == 0);
}

TEST_CASE("Actors can be iterated over by family.", "[actor]") {
    ActorManager manager;
    manager.addComponent<Test1Component>(manager.createActor());
    manager.createActor();
    manager.addComponent<Test1Component>(manager.createActor());
    manager.createActor();
    manager.addComponent<Test1Component>(manager.createActor());
    manager.createActor();

    int loops = 0;
    manager.getActorsFor(Family::all<Test1Component>().get())
        .each([&loops](Actor_t actor) -> void {
        loops++;
    });
    REQUIRE(loops == 3);
}
TEST_CASE("ActorManager can remove all actors in a family.", "[actors]") {
    ActorManager manager;
    manager.addComponent<Test1Component>(manager.createActor());
    manager.createActor();
    manager.addComponent<Test1Component>(manager.createActor());
    manager.createActor();
    manager.addComponent<Test1Component>(manager.createActor());
    manager.createActor();
    REQUIRE(manager.getActors().count() == 6);
    REQUIRE(manager.getActorsFor(Family::all<Test1Component>().get()).count() == 3);
    manager.destroyAllActorsFor(Family::all<Test1Component>().get());
    REQUIRE(manager.getActorsFor(Family::all<Test1Component>().get()).count() == 0);
    REQUIRE(manager.getActors().count() == 3);
}
