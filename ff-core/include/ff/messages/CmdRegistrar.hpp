/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_MESSAGES_CMD_REGISTRAR_HPP
#define _FF_MESSAGES_CMD_REGISTRAR_HPP

#include <string>
#include <functional>
#include <unordered_map>
#include <set>

#include <ff/io/Serializer.hpp>
#include <ff/io/MetadataSerializer.hpp>
#include <ff/messages/Cmd.hpp>

namespace ff {

class CmdRegistrar {
public:
    static void registerCmd(std::string const& name,
            std::string const& description,
            std::function<void(Serializer&)> dispatcher, std::function<void(Serializer&)> serializer);

    static std::set<std::string> getCmds();
    static std::vector<std::string> getCmdsForTerm(std::string const& term);
    static bool dispatchCmd(std::string const& name, Serializer& serializer);
    static bool analyzeCmd(std::string const& name, MetadataSerializer& serializer);
    static bool getCmdDescription(std::string const& name, std::string* const& descOut);

private:
    static std::set<std::string>& getNames();
    static std::unordered_map<std::string, std::function<void(Serializer&)>>& getDispatchers();
    static std::unordered_map<std::string, std::function<void(Serializer&)>>& getSerializers();
    static std::unordered_map<std::string, std::string>& getDescriptions();
};

}

#include <ff/Locator.hpp>

#define FF_CMD_REGISTER(sname) \
    namespace { \
        inline void FF_CMD_REGISTER_##sname##_helper() { \
            static bool registered = false; \
            if (!registered) { \
                ff::CmdRegistrar::registerCmd(sname::getCmdName(), sname::getDescription(), \
                    [](ff::Serializer& serializer) -> void { \
                    auto ret = ff::Locator::getMessageBus().dispatch<sname>(serializer); \
                    if(ret) { \
                        std::string strgfy = ret->stringify(); \
                        if(strgfy.size() > 0) { \
                            FF_CONSOLE_LOG("%s", strgfy); \
                        } \
                    } \
                }, [](ff::Serializer& serializer) -> void { \
                    sname cmd; \
                    cmd.serialize(serializer); \
                }); \
                registered = true; \
            } \
        } \
        struct FF_CMD_REGISTER_##sname##_caller { \
            FF_CMD_REGISTER_##sname##_caller() { \
                FF_CMD_REGISTER_##sname##_helper(); \
            } \
        }; \
        static FF_CMD_REGISTER_##sname##_caller FF_CMD_REGISTER_##sname##_instance; \
    }

#endif

