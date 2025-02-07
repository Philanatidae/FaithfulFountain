/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_MESSAGES_CMD_HANDLER_HPP
#define _FF_MESSAGES_CMD_HANDLER_HPP

#include <ff/messages/Cmd.hpp>

#include <memory> 

namespace ff {

class MessageBus;

class ICmdHandler {
friend class MessageBus;

public:
    ICmdHandler();
    virtual ~ICmdHandler();

private:
    MessageBus* _bus;
};

template<typename C,
    typename std::enable_if<std::is_base_of<Cmd<typename C::Ret>, C>::value>::type* En = nullptr>
class CmdHandler : public ICmdHandler {
public:
    virtual std::unique_ptr<typename C::Ret> handleCmd(C const& cmd) = 0;
private:
};

}

#endif

