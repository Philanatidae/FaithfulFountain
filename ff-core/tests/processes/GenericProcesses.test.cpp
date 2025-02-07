/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <catch2/catch_test_macros.hpp>

#include <ff/processes/Process.hpp>
#include <ff/processes/ProcessManager.hpp>
#include <ff/processes/DelegateProcess.hpp>
#include <ff/processes/WaitProcess.hpp>
#include <ff/processes/IntervalProcess.hpp>

#include <ff/util/Macros.hpp>

using namespace ff;

class TestProcess : public Process {
public:
    TestProcess() {
    }
    ~TestProcess() {
    }
};

TEST_CASE("DelegateProcess calls its delegate function when triggered.", "[processes]") {
    ff::ProcessManager manager;

    bool hasTriggered = false;
    manager.attachProcess(std::make_shared<ff::DelegateProcess>([&]() {
        hasTriggered = true;
    }));
    manager.tick(-1);

    REQUIRE(hasTriggered);
}

TEST_CASE("Processes are not initialized immediately when attached.", "[processes]") {
    ff::ProcessManager manager;

    bool hasTriggered = false;
    auto process = std::make_shared<TestProcess>();
    manager.attachProcess(process);

    REQUIRE(!process->getInitialized());
}
TEST_CASE("Processes are initialized on attach with flag.", "[processes]") {
    ff::ProcessManager manager;

    bool hasTriggered = false;
    auto process = std::make_shared<TestProcess>();
    manager.attachProcess(process, ProcessFlags::INITIALIZE_ON_ATTACH);

    REQUIRE(process->getInitialized());
}

TEST_CASE("WaitProcess ends after a specified duration.", "[processes]") {
    ff::ProcessManager manager;

    bool hasTriggered = false;
    manager.attachProcess(std::make_shared<ff::WaitProcess>(10))->setNext(std::make_shared<ff::DelegateProcess>([&]() {
        hasTriggered = true;
    }));

    manager.tick(11);

    REQUIRE(hasTriggered);
}

class DummyTestIntervalProcess: public ff::IntervalProcess {
public:
    DummyTestIntervalProcess(const float& interval)
        :IntervalProcess(interval) {
    }
    ~DummyTestIntervalProcess() {
    }

    int tickCount = 0;

protected:
    inline void onTick(const float& dt) override {
        FF_UNUSED(dt);

        tickCount++;
    }
};

TEST_CASE("IntervalProcess ticks after a specified interval.", "[processes]") {
    ff::ProcessManager manager;

    auto intervalProcess = std::make_shared<DummyTestIntervalProcess>(10);
    manager.attachProcess(intervalProcess);

    manager.tick(5);
    REQUIRE(intervalProcess->tickCount == 0);

    manager.tick(6);
    REQUIRE(intervalProcess->tickCount == 1);

    manager.tick(5);
    REQUIRE(intervalProcess->tickCount == 1);

    manager.tick(5);
    REQUIRE(intervalProcess->tickCount == 2);
}
