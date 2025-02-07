/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/CommandLineOptions.hpp>

namespace ff {
    CommandLineOptions::CommandLineOptions() {
    }
    CommandLineOptions::CommandLineOptions(int argc, char** argv, const int& offset) {
        std::vector<std::string> args;
        for(int i = offset; i < argc; i++) {
            args.push_back(std::string(argv[i]));
        }
        parse(args);
    }
    CommandLineOptions::CommandLineOptions(const std::vector<std::string>& args) {
        parse(args);
    }
    CommandLineOptions::~CommandLineOptions() {
    }

    void CommandLineOptions::parse(const std::vector<std::string>& args) {
        for(auto it = args.begin();
            it != args.end();
            it++) {
            const auto& arg = *it;
            if(arg[0] == '-') {
                parseOption(arg);
                continue;
            }
            if(std::regex_search(arg, std::regex("[A-Za-z-]+"))) {
                _cmdList.push_back(arg);
                continue;
            }
            FF_CONSOLE_ERROR("Unexpected token: `%s`.", arg[0]);
        }
    }
    void CommandLineOptions::parseOption(const std::string& arg) {
        switch(arg[1]) {
        case 'D':
            parseCVarAssignment(std::string(&arg.c_str()[2]));
            break;
        default:
            FF_CONSOLE_ERROR("Unknown flag \"-%s\"", arg[1]);
            break;
        }
    }
    void CommandLineOptions::parseCVarAssignment(const std::string& statement) {
        if(!std::regex_match(statement, std::regex("[A-z]+=.+"))) {
            FF_CONSOLE_ERROR("CVar assignment must be in the form `name=value`.");
            return;
        }

        const auto equalIdx = statement.find_first_of('=');
        const std::string name = std::string(statement, 0, equalIdx);
        const std::string value = std::string(statement, equalIdx + 1);
        _cvarNameValueList.push_back(std::make_pair(name, value));
    }

    const std::vector<std::pair<std::string, std::string>>& CommandLineOptions::getCVarNameValueList() const {
        return _cvarNameValueList;
    }
    std::vector<std::string> CommandLineOptions::getCommandList() const {
        return _cmdList;
    }
}
