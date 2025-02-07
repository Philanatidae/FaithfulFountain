/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <catch2/catch_test_macros.hpp>

#include <memory>

#include <ff/processes/ProcessManager.hpp>
#include <ff/processes/Process.hpp>

#include <ff/util/Macros.hpp>

class DummyTestProcess : public ff::Process {
public:
    DummyTestProcess() {
    }
    ~DummyTestProcess() {
    }

    int initializeCallCount = 0;
    bool didInitialize = false;
    bool didPreUpdate = false;
    int updateCallCount = 0;
    bool didUpdate = false;
    bool didPostUpdate = false;
    bool didKill = false;
    bool didSoftKill = false;
    bool didTogglePause = false;

protected:
    inline void onInitialize() override {
        didInitialize = true;
        initializeCallCount++;
    }
    inline void onPreUpdate() override {
        didPreUpdate = true;
    }
    inline void onUpdate(const float& dt) override {
        FF_UNUSED(dt);

        updateCallCount++;
        didUpdate = true;
    }
    inline void onPostUpdate() override {
        didPostUpdate = true;
    }
    inline void onKill() override {
        didKill = true;
    }
    inline void onSoftKill() override {
        didSoftKill = true;
    }
    inline void onTogglePause() override {
        didTogglePause = true;
    }
};

TEST_CASE("Process functions are called by ProcessManager.", "[processes]") {
    ff::ProcessManager processManager;
    auto process = std::make_shared<DummyTestProcess>();
    processManager.attachProcess(process);

    processManager.tick(0);
    REQUIRE(process->didInitialize);
    REQUIRE(process->didPreUpdate);
    REQUIRE(process->didUpdate);
    REQUIRE(process->didPostUpdate);

    processManager.killAll();
    REQUIRE(process->didSoftKill);
}

TEST_CASE("ProcessManager can kill all functions soft or hard.", "[processes]") {
    ff::ProcessManager processManager;
    auto process = std::make_shared<DummyTestProcess>();
    processManager.attachProcess(process);

    SECTION("ProcessManager soft kills by default.") {
        processManager.killAll();
        REQUIRE(process->didSoftKill);
        REQUIRE(!process->didKill);
    }

    SECTION("ProcessManager can optionally kill hard.") {
        processManager.killAll(true);
        REQUIRE(process->didKill);
        REQUIRE(!process->didSoftKill);
    }
}

TEST_CASE("Process initialize is only called once.", "[processes]") {
    ff::ProcessManager processManager;
    auto process = std::make_shared<DummyTestProcess>();
    processManager.attachProcess(process);

    processManager.tick(0);
    processManager.tick(0);
    REQUIRE(process->initializeCallCount == 1);
    REQUIRE(process->updateCallCount == 2);
}

class DummyKillTestProcess : public ff::Process {
public:
    DummyKillTestProcess() {
    }
    ~DummyKillTestProcess() {
    }

    bool didKill = false;

protected:
    inline void onKill() override {
        didKill = true;
    }
};

TEST_CASE("Default Process behavior is to kill on soft kill.", "[processes]") {
    ff::ProcessManager processManager;
    auto process = std::make_shared<DummyKillTestProcess>();
    processManager.attachProcess(process);

    processManager.tick(0);
    processManager.killAll();
    REQUIRE(process->didKill);
}
