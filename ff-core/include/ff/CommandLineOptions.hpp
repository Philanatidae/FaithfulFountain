/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMMAND_LINE_OPTIONS_HPP
#define _FAITHFUL_FOUNTAIN_COMMAND_LINE_OPTIONS_HPP

#include <vector>
#include <string>
#include <utility>
#include <ff/Console.hpp>
#include <regex>

namespace ff {
    class CommandLineOptions {
    public:
        CommandLineOptions();
        CommandLineOptions(int argc, char* argv[], const int& offset = 0);
        CommandLineOptions(const std::vector<std::string>& args);
        ~CommandLineOptions();

        const std::vector<std::pair<std::string, std::string>>& getCVarNameValueList() const;
        std::vector<std::string> getCommandList() const;
    private:
        void parse(const std::vector<std::string>& args);
        void parseOption(const std::string& arg);
        void parseCVarAssignment(const std::string& statement);

        std::vector<std::pair<std::string, std::string>> _cvarNameValueList;
        std::vector<std::string> _cmdList;
    };
}

#endif
