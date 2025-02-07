/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/messages/CmdRegistrar.hpp>

#include <ff/Console.hpp>

namespace ff {

void CmdRegistrar::registerCmd(std::string const& name,
    std::string const& description,
    std::function<void(Serializer&)> dispatcher, std::function<void(Serializer&)> serializer) {
    if(getNames().find(name) != getNames().end()) {
        // Ignore
        return;
    }
    getNames().insert(name);

    if(getDispatchers().find(name) != getDispatchers().end()) {
        // Ignore
        return;
    }
    getDispatchers().emplace(name, dispatcher);

    if(getSerializers().find(name) != getSerializers().end()) {
        // Ignore
        return;
    }
    getSerializers().emplace(name, serializer);

    if(getDescriptions().find(name) != getDescriptions().end()) {
        // Ignore
        return;
    }
    getDescriptions().emplace(name, description);
}

std::set<std::string> CmdRegistrar::getCmds() {
    return getNames();
}
std::vector<std::string> CmdRegistrar::getCmdsForTerm(std::string const& term) {
    std::vector<std::string> cmds;
    for(auto const& name : getNames()) {
        if(name.find(term) != std::string::npos) {
            cmds.push_back(name);
        }
    }
    return cmds;
}
bool CmdRegistrar::dispatchCmd(std::string const& name, Serializer& serializer) {
    if(getDispatchers().find(name) == getDispatchers().end()) {
        return false;
    }

    getDispatchers()[name](serializer);

    return true;
}
bool CmdRegistrar::analyzeCmd(std::string const& name, MetadataSerializer& serializer) {
    if(getSerializers().find(name) == getSerializers().end()) {
        return false;
    }

    getSerializers()[name](serializer);

    return true;
}
bool CmdRegistrar::getCmdDescription(std::string const& name, std::string* const& descOut) {
    if(getDescriptions().find(name) == getDescriptions().end()) {
        return false;
    }

    if(descOut != nullptr) {
        *descOut = getDescriptions()[name];
    }

    return true;
}

std::set<std::string>& CmdRegistrar::getNames() {
    static std::set<std::string> names;
    return names;
}
std::unordered_map<std::string, std::function<void(Serializer&)>>& CmdRegistrar::getDispatchers() {
    static std::unordered_map<std::string, std::function<void(Serializer&)>> dispatchers;
    return dispatchers;
}
std::unordered_map<std::string, std::function<void(Serializer&)>>& CmdRegistrar::getSerializers() {
    static std::unordered_map<std::string, std::function<void(Serializer&)>> serializers;
    return serializers;
}
std::unordered_map<std::string, std::string>& CmdRegistrar::getDescriptions() {
    static std::unordered_map<std::string, std::string> descriptions;
    return descriptions;
}

}

