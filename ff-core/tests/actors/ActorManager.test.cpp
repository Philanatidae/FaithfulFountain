/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <catch2/catch_test_macros.hpp>

#include <ff/actors/ActorManager.hpp>

using namespace ff;

TEST_CASE("ActorManager can create actors.", "[actors]") {
    ActorManager manager;
    Actor_t actor = manager.createActor();
    REQUIRE(!isActorNull(actor));
    REQUIRE(manager.getActors().count() == 1);
}

TEST_CASE("ActorManager can remove actors by actor.", "[actors]") {
    ActorManager manager;
    Actor_t actor = manager.createActor();
    manager.destroyActor(actor);
    REQUIRE(manager.getActors().count() == 0);
}
TEST_CASE("ActorManager can remove all actors.", "[actors]") {
    ActorManager manager;
    manager.createActor();
    manager.createActor();
    manager.createActor();
    manager.destroyAllActors();
    REQUIRE(manager.getActors().count() == 0);
}

TEST_CASE("ActorManager recycles IDs.", "[actors]") {
    ActorManager manager;
    Actor_t actor = manager.createActor();
    ActorID actorID = convertActorToID(actor);
    manager.destroyActor(actor);
    REQUIRE(convertActorToID(manager.createActor()) == actorID);
}

TEST_CASE("ActorManager can iterate over all actors.", "[actors]") {
    ActorManager manager;
    Actor_t actor1 = manager.createActor();
    Actor_t actor2 = manager.createActor();
    Actor_t actor3 = manager.createActor();
    bool actor1Check = false,
        actor2Check = false,
        actor3Check = false;
    int count = 0;
    manager.getActors().each([&](Actor_t actor) {
        if(actor == actor1) {
            actor1Check = true;
        } else if(actor == actor2) {
            actor2Check = true;
        } else if(actor == actor3) {
            actor3Check = true;
        }
        count++;
    });
    REQUIRE(actor1Check);
    REQUIRE(actor2Check);
    REQUIRE(actor3Check);
    REQUIRE(count == 3);
}

TEST_CASE("Actor version is increased when ID is recycled.", "[actors]") {
    ActorManager manager;
    Actor_t actor = manager.createActor();
    ActorVersion actorVersion = convertActorToVersion(actor);
    manager.destroyActor(actor);
    REQUIRE(convertActorToVersion(manager.createActor()) == actorVersion + 1);
}
TEST_CASE("When an actor is destroyed, it is no longer valid even if the ID is recycled.", "[actors]") {
    ActorManager manager;
    Actor_t actor = manager.createActor();
    ActorID actorID = convertActorToID(actor);
    REQUIRE(manager.isActorAlive(actor));
    manager.destroyActor(actor);
    REQUIRE(!manager.isActorAlive(actor));
    
    Actor_t actor2 = manager.createActor();
    REQUIRE(convertActorToID(actor2) == actorID);
    REQUIRE(!manager.isActorAlive(actor));
}

TEST_CASE("Destroyed actors are not iterated over.", "[actors]") {
    ActorManager manager;
    Actor_t actor1 = manager.createActor();
    Actor_t actor2 = manager.createActor();
    Actor_t actor3 = manager.createActor();
    Actor_t actor4 = manager.createActor();
    manager.createActor();
    int count = 0;
    manager.getActors().each([&](Actor_t actor) {
        count++;
    });
    REQUIRE(count == 5);

    SECTION("All actors destroyed.") {
        manager.destroyAllActors();
        count = 0;
        manager.getActors().each([&](Actor_t actor) {
            count++;
        });
        REQUIRE(count == 0);
    }
    SECTION("Partial actors destroyted.") {
        manager.destroyActor(actor3);
        manager.destroyActor(actor4);
        count = 0;
        manager.getActors().each([&](Actor_t actor) {
            count++;
        });
        REQUIRE(count == 3);
    }
}
TEST_CASE("Sets can report if they contain an actor.", "[actors]") {
    ActorManager manager;
    Actor_t actor1 = manager.createActor();
    Actor_t actor2 = manager.createActor();
    Actor_t actor3 = manager.createActor();
    REQUIRE(manager.getActors().contains(actor1));
    REQUIRE(manager.getActors().contains(actor2));
    REQUIRE(manager.getActors().contains(actor3));
    manager.destroyActor(actor2);
    REQUIRE(manager.getActors().contains(actor1));
    REQUIRE(!manager.getActors().contains(actor2));
    REQUIRE(manager.getActors().contains(actor3));
}
