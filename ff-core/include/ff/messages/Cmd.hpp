/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_MESSAGES_CMD_HPP
#define _FF_MESSAGES_CMD_HPP

#include <string>
#include <tinyformat/tinyformat.h>
#include <unordered_map>
#include <functional>
#include <ff/io/Serializer.hpp>

namespace ff {

struct CmdRet {
    virtual std::string stringify() const = 0;
};
template<typename RetT>
class Cmd {
public:
    virtual ~Cmd() = default;

    virtual std::string getName() const = 0;

    virtual std::string stringify() const = 0;

    virtual void serialize(Serializer& serializer) = 0;

    using Ret = RetT;
};

}

#endif

