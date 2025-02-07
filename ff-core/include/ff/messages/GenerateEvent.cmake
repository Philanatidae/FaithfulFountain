# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(FF_EVENT_DEFINE_MACROS "")

foreach(i RANGE 1 ${FF_MESSAGE_MAX_MEMBER_VARIABLES})
    set(_FF_EVENT_DEFINE_MACRO "#define FF_EVENT_DEFINE_${i}(name, event_str")
    set(_FF_EVENT_DEFINE_MACRO_ARGS "")
    set(_FF_EVENT_DEFINE_MACRO_MEMBERS "")
    set(_FF_EVENT_DEFINE_MACRO_INITS "")
    set(_FF_EVENT_DEFINE_MACRO_FORMAT_ARGS "")

    foreach(j RANGE 1 ${i})
        set(_FF_EVENT_DEFINE_MACRO "${_FF_EVENT_DEFINE_MACRO}, type${j}, var${j}")

        if(j GREATER 1)
            set(_FF_EVENT_DEFINE_MACRO_ARGS "${_FF_EVENT_DEFINE_MACRO_ARGS}, ")
            set(_FF_EVENT_DEFINE_MACRO_INITS "${_FF_EVENT_DEFINE_MACRO_INITS}, ")
            set(_FF_EVENT_DEFINE_MACRO_FORMAT_ARGS "${_FF_EVENT_DEFINE_MACRO_FORMAT_ARGS}, ")
        endif()

        set(_FF_EVENT_DEFINE_MACRO_ARGS "${_FF_EVENT_DEFINE_MACRO_ARGS} type${j} const& var${j}")
        set(_FF_EVENT_DEFINE_MACRO_MEMBERS "${_FF_EVENT_DEFINE_MACRO_MEMBERS} type${j} var${j};")
        set(_FF_EVENT_DEFINE_MACRO_INITS "${_FF_EVENT_DEFINE_MACRO_INITS} var${j}(var${j})")
        set(_FF_EVENT_DEFINE_MACRO_FORMAT_ARGS "${_FF_EVENT_DEFINE_MACRO_FORMAT_ARGS} var${j}")
    endforeach()


    string(STRIP "${_FF_EVENT_DEFINE_MACRO_INITS}" _FF_EVENT_DEFINE_MACRO_INITS)
    string(STRIP "${_FF_EVENT_DEFINE_MACRO_ARGS}" _FF_EVENT_DEFINE_MACRO_ARGS)

    set(_FF_EVENT_DEFINE_MACRO "${_FF_EVENT_DEFINE_MACRO}, format_str, ...) \\\n\
    struct name final : public ff::Event { \\\n\
${_FF_EVENT_DEFINE_MACRO_MEMBERS}\\\n\
name(${_FF_EVENT_DEFINE_MACRO_ARGS}) : ${_FF_EVENT_DEFINE_MACRO_INITS} {} \\\n\
_FF_EVENT_DEFINE_STRINGIFY(format_str, __VA_ARGS__) \\\n\
static char const* getEventName() { \\\n\
    return event_str; \\\n\
} \\\n\
std::string getName() const override { \\\n\
    return getEventName(); \\\n\
} \\\n\
};\n")

    set(FF_EVENT_DEFINE_MACROS "${FF_EVENT_DEFINE_MACROS}${_FF_EVENT_DEFINE_MACRO}\n")
endforeach()

# Add FF_EVENT_DEFINE_0 macro
set(FF_EVENT_DEFINE_MACROS "#define FF_EVENT_DEFINE_0(name, event_str) \\\n\
struct name final : public ff::Event { \\\n\
name() {} \\\n\
std::string stringify() const override { \\\n\
    return \"\"; \\\n\
} \\\n\
static char const* getEventName() { \\\n\
    return event_str; \\\n\
} \\\n\
std::string getName() const override { \\\n\
    return getEventName(); \\\n\
} \\\n\
};\n${FF_EVENT_DEFINE_MACROS}")

configure_file(include/ff/messages/Event.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/ff/messages/Event.hpp)

