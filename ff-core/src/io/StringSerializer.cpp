/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/io/StringSerializer.hpp>

#include <iomanip>
#include <ff/Console.hpp>

namespace ff {

StringSerializer::StringSerializer()
    :_direction(SerializerDirection::WRITE),
    _stream(&_strBuf) {
}

SerializerDirection StringSerializer::getDirection() const {
    return _direction;
}
void StringSerializer::setDirection(SerializerDirection const& dir) {
    _direction = dir;
    _stream.seekg(0);
    _stream.seekp(0);
    _elementCount = 0;
}

void StringSerializer::endObject() {
    // @todo
}

std::string StringSerializer::getString() const {
    return _strBuf.str();
}
std::string StringSerializer::getRemainderFromHead() {
    std::istream::pos_type currentPosition = _stream.tellg();
    std::string remainder;
    std::getline(_stream, remainder, '\0'); // Read the entire content from the current position until the null terminator
    _stream.seekg(currentPosition);
    return remainder;
}
void StringSerializer::setString(std::string const& str) {
    _strBuf.str(str);
    _stream.seekg(0);
    _stream.seekp(0);

    _elementCount = 0;
    bool in_quotes = false;
    bool escape_next = false;
    int word_start = -1;

    for (size_t i = 0; i < str.size(); ++i) {
        if (!escape_next && str[i] == '\\') {
            escape_next = true;
            continue;
        }

        if (!escape_next && str[i] == '\"') {
            in_quotes = !in_quotes;
            if (!in_quotes && word_start != -1) {
                _elementCount++;
                word_start = -1;
            }
            continue;
        }

        if (!in_quotes && str[i] == ' ' && word_start != -1) {
            _elementCount++;
            word_start = -1;
            continue;
        }

        if (word_start == -1 && (in_quotes || str[i] != ' ')) {
            word_start = i;
        }

        escape_next = false;
    }

    if (word_start != -1) {
        _elementCount++;
    }
}

int StringSerializer::getElementCount() const {
    return _elementCount;
}

void StringSerializer::serializeImpl(char const* const label,
    uint8_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        handleExists(exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            _stream << x << ' ';
            _elementCount++;
        } else {
            _stream >> x;
        }
    }
}
void StringSerializer::serializeImpl(char const* const label,
    uint16_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        handleExists(exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            _stream << x << ' ';
            _elementCount++;
        } else {
            _stream >> x;
        }
    }
}
void StringSerializer::serializeImpl(char const* const label,
    uint32_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        handleExists(exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            _stream << x << ' ';
            _elementCount++;
        } else {
            _stream >> x;
        }
    }
}
void StringSerializer::serializeImpl(char const* const label,
    uint64_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        handleExists(exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            _stream << x << ' ';
            _elementCount++;
        } else {
            _stream >> x;
        }
    }
}

void StringSerializer::serializeImpl(char const* const label,
    int8_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        handleExists(exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            _stream << x << ' ';
            _elementCount++;
        } else {
            _stream >> x;
        }
    }
}
void StringSerializer::serializeImpl(char const* const label,
    int16_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        handleExists(exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            _stream << x << ' ';
            _elementCount++;
        } else {
            _stream >> x;
        }
    }
}
void StringSerializer::serializeImpl(char const* const label,
    int32_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        handleExists(exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            _stream << x << ' ';
            _elementCount++;
        } else {
            _stream >> x;
        }
    }
}
void StringSerializer::serializeImpl(char const* const label,
    int64_t& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        handleExists(exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            _stream << x << ' ';
            _elementCount++;
        } else {
            _stream >> x;
        }
    }
}

void StringSerializer::serializeImpl(char const* const label,
    float& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        handleExists(exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            _stream << std::setprecision(9) << x << ' ';
            _elementCount++;
        } else {
            _stream >> x;
        }
    }
}
void StringSerializer::serializeImpl(char const* const label,
    double& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        handleExists(exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            _stream << std::setprecision(17) << x << ' ';
            _elementCount++;
        } else {
            _stream >> x;
        }
    }
}

void StringSerializer::serializeImpl(char const* const label,
    bool& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        handleExists(exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            _stream << (x ? "1 " : "0 ");
            _elementCount++;
        } else {
            std::string temp;
            _stream >> temp;

            // Convert to lowercase
            std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

            if(temp == "1"
                || temp[0] == 'y'
                || temp[0] == 't'
                || temp == "on") {
                x = true;
            } else if(temp == "1"
                || temp[0] == 'n'
                || temp[0] == 'f'
                || temp == "off") {
                x = false;
            } else {
                FF_CONSOLE_FATAL("Invalid boolean value in serialized data.");
            }
        }
    }
}

void StringSerializer::serializeImpl(char const* const label,
    std::string& x,
    bool const& optional,
    bool& exists) {
    if(optional) {
        handleExists(exists);
    }
    if(exists) {
        if(_direction == SerializerDirection::WRITE) {
            _stream << std::quoted(x) << ' ';
            _elementCount++;
        } else {
            _stream >> std::quoted(x);
        }
    }
}


void StringSerializer::beginObjectImpl(char const* const label,
    bool const& optional,
    bool& exists) {
    // @todo
}

void StringSerializer::handleExists(bool& exists) {
    if(getDirection() == SerializerDirection::WRITE) {
        if(!exists) {
            _stream << "? ";
        }
    } else {
        // Check if we are at the end of the stream or there are only whitespaces left
        char c;
        bool foundNonWhitespace = false;
        while (_stream.get(c)) {
            if (c == '?') {
                exists = false;
                break;
            } else if (!isspace(c)) {
                foundNonWhitespace = true;
                _stream.putback(c);
                break;
            }
        }

        // If we are at the end of the stream, or there are only whitespaces left,
        // or we found a '?' character, set exists to false.
        if (!foundNonWhitespace || c == '?') {
            exists = false;
        } else {
            exists = true;
        }
    }
}

}
