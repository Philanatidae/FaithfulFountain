/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <catch2/catch_test_macros.hpp>

#include <ff/messages/MessageBus.hpp>
#include <ff/messages/EventListener.hpp>
#include <ff/messages/Event.hpp>
#include <ff/messages/CmdHandler.hpp>
#include <ff/messages/CmdHelpers.hpp>

#include <ff/util/Macros.hpp>

#include <ff/Locator.hpp>

class TestEvent: public ff::Event {
public:
    TestEvent() {
    }
    ~TestEvent() {
    }

    static char const* getEventName() {
        return "evt_test";
    }

    inline std::string getName() const override {
        return getEventName();
    }

    inline std::string stringify() const override {
        return "";
    }
};

class TestEventListener: public ff::EventListener<TestEvent> {
public:
    TestEventListener() {
        ff::Locator::getMessageBus().addListener<TestEvent>(this);
    }
    ~TestEventListener() {
    }

    bool receivedEvent = false;

    inline bool processEvent(TestEvent const& testEvent) override {
        FF_UNUSED(testEvent);

        receivedEvent = true;

        return false;
    }
};

TEST_CASE("A message listener receives a specific event when dispatched.", "[messages]") {
    TestEventListener listener;
    ff::Locator::getMessageBus().dispatch<TestEvent>();
    REQUIRE(listener.receivedEvent);
}

TEST_CASE("Removed message listener no longer receives an event.", "[messages]") {
    TestEventListener listener;
    ff::Locator::getMessageBus().removeListener<TestEvent>(&listener);
    ff::Locator::getMessageBus().dispatch<TestEvent>();
    REQUIRE(!listener.receivedEvent);
}

class TestEvent2: public ff::Event {
public:
    TestEvent2() {
    }
    ~TestEvent2() {
    }

    static char const* getEventName() {
        return "evt_test2";
    }

    inline std::string getName() const override {
        return getEventName();
    }
    inline std::string stringify() const override {
        return "";
    }
};

class TestEventListener2: public ff::EventListener<TestEvent2> {
public:
    TestEventListener2() {
        ff::Locator::getMessageBus().addListener<TestEvent2>(this);
    }
    ~TestEventListener2() {
    }

    bool receivedEvent = false;

    inline bool processEvent(TestEvent2 const& testEvent) override {
        FF_UNUSED(testEvent);

        receivedEvent = true;

        return false;
    }
};

class CombinedListener : public TestEventListener, public TestEventListener2 {
};

TEST_CASE("Combined listeners can selectively listen.", "[messages]") {
    CombinedListener listener;
    ff::Locator::getMessageBus().removeListener<TestEvent>(&listener);
    ff::Locator::getMessageBus().dispatch<TestEvent>();
    ff::Locator::getMessageBus().dispatch<TestEvent2>();
    TestEventListener& testListener = listener;
    TestEventListener2& test2Listener = listener;
    REQUIRE(!testListener.receivedEvent);
    REQUIRE(test2Listener.receivedEvent);
}

class AllListener: public ff::GenericEventListener {
public:
    AllListener(const std::string& messageName) {
        ff::Locator::getMessageBus().addListener(messageName, this);
    }
    ~AllListener() {
    }

    bool receivedEvent = false;
    std::string receivedEventName = "";

    inline bool processEvent(const std::shared_ptr<ff::Event>& message) {
        receivedEvent = true;
        receivedEventName = message->getName();

        return false;
    }
};

TEST_CASE("Generic listeners can listen to messages based on names.", "[messages]") {
    AllListener listener("evt_test");
    ff::Locator::getMessageBus().dispatch<TestEvent>();
    REQUIRE(listener.receivedEvent);
    REQUIRE(listener.receivedEventName == "evt_test");
}

TEST_CASE("Wildcard listeners listen for all events.", "[messages]") {
    AllListener listener(ff::MESSAGE_WILDCARD);
    ff::Locator::getMessageBus().dispatch<TestEvent>();
    REQUIRE(listener.receivedEvent);
    REQUIRE(listener.receivedEventName == "evt_test");
    ff::Locator::getMessageBus().dispatch<TestEvent2>();
    REQUIRE(listener.receivedEventName == "evt_test2");
}

FF_CMD_DEFINE_0_R1(TestCmd,
    "cmd_test",
    "Test",
    bool, val,
    "%s",
    (val));

class TestCmdListener: public ff::CmdHandler<TestCmd> {
public:
    TestCmdListener() {
        ff::Locator::getMessageBus().addHandler<TestCmd>(this);
    }
    ~TestCmdListener() {
    }

    bool receivedCommand = false;

    inline std::unique_ptr<typename TestCmd::Ret> handleCmd(TestCmd const& cmd) override {
        receivedCommand = true;

        auto ret = std::make_unique<typename TestCmd::Ret>(true);
        return ret;
    }
};

TEST_CASE("Commands dispatched without a handler is not an error.", "[messages]") {
    REQUIRE(ff::Locator::getMessageBus().dispatch<TestCmd>() == nullptr);
}
TEST_CASE("Commands are procesed as soon as they are dispatched.", "[messages]") {
    TestCmdListener handler;
    REQUIRE(ff::Locator::getMessageBus().dispatch<TestCmd>() != nullptr);
    REQUIRE(handler.receivedCommand);
}
TEST_CASE("Commands can return values.", "[messages]") {
    TestCmdListener handler;
    auto ret = ff::Locator::getMessageBus().dispatch<TestCmd>();
    REQUIRE(ret != nullptr);
    REQUIRE(ret->val);
    REQUIRE(handler.receivedCommand);
}

class ConsumingEventListener: public ff::EventListener<TestEvent> {
public:
    ConsumingEventListener() {
        ff::Locator::getMessageBus().addListener<TestEvent>(this, ff::MessageListenerPriority::HIGH);
    }
    ~ConsumingEventListener() = default;

    bool receivedEvent = false;

    inline bool processEvent(TestEvent const& testEvent) override {
        receivedEvent = true;

        return true;
    }
};

TEST_CASE("Listeners with higher priority can consume events and stop propogation.", "[messages]") {
    TestEventListener listenerLow;
    ConsumingEventListener listenerHigh;
    REQUIRE(!listenerLow.receivedEvent);
    REQUIRE(!listenerHigh.receivedEvent);
    ff::Locator::getMessageBus().dispatch<TestEvent>();
    REQUIRE(!listenerLow.receivedEvent);
    REQUIRE(listenerHigh.receivedEvent);
}
