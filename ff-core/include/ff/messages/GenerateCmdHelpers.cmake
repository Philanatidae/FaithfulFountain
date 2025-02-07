# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(FF_CMD_DEFINE_MACROS "// GENERATED! DO NOT MODIFY!\n")

foreach(i RANGE 0 ${FF_MESSAGE_MAX_MEMBER_VARIABLES})
    foreach(r RANGE 0 ${FF_MESSAGE_MAX_MEMBER_VARIABLES})
        set(_FF_CMD_DEFINE_MACRO "#define FF_CMD_DEFINE_${i}_R${r}(name, cmd_str, desc_str")
        set(_FF_CMD_DEFINE_MACRO_ARGS "")
        set(_FF_CMD_DEFINE_MACRO_MEMBERS "")
        set(_FF_CMD_DEFINE_MACRO_INITS "")
        set(_FF_CMD_DEFINE_MACRO_SERIALIZERS "")
        set(_FF_CMD_DEFINE_RET_MEMBERS "")
        set(_FF_CMD_DEFINE_RET_INITS "")
        set(_FF_CMD_DEFINE_RET_ARGS "")

        if(i GREATER 0)
            foreach(j RANGE 1 ${i})
                set(_FF_CMD_DEFINE_MACRO "${_FF_CMD_DEFINE_MACRO}, type${j}, var${j}")

                if(j GREATER 1)
                    set(_FF_CMD_DEFINE_MACRO_ARGS "${_FF_CMD_DEFINE_MACRO_ARGS}, ")
                    set(_FF_CMD_DEFINE_MACRO_INITS "${_FF_CMD_DEFINE_MACRO_INITS}, ")
                endif()

                set(_FF_CMD_DEFINE_MACRO_ARGS "${_FF_CMD_DEFINE_MACRO_ARGS} type${j} const& var${j}")
                set(_FF_CMD_DEFINE_MACRO_MEMBERS "${_FF_CMD_DEFINE_MACRO_MEMBERS} type${j} var${j};")
                set(_FF_CMD_DEFINE_MACRO_INITS "${_FF_CMD_DEFINE_MACRO_INITS} var${j}(var${j})")
                set(_FF_CMD_DEFINE_MACRO_SERIALIZERS "${_FF_CMD_DEFINE_MACRO_SERIALIZERS} serializer.serialize(#var${j}, var${j});")
            endforeach()
            set(_FF_CMD_DEFINE_MACRO "${_FF_CMD_DEFINE_MACRO}, format_str, format_args")
        endif()

        if(r GREATER 0)
            foreach(k RANGE 1 ${r})
                set(_FF_CMD_DEFINE_MACRO "${_FF_CMD_DEFINE_MACRO}, retType${k}, retVar${k}")

                if(k GREATER 1)
                    set(_FF_CMD_DEFINE_RET_INITS "${_FF_CMD_DEFINE_RET_INITS}, ")
                    set(_FF_CMD_DEFINE_RET_ARGS "${_FF_CMD_DEFINE_RET_ARGS}, ")
                endif()

                set(_FF_CMD_DEFINE_RET_MEMBERS "${_FF_CMD_DEFINE_RET_MEMBERS} retType${k} retVar${k};")
                set(_FF_CMD_DEFINE_RET_INITS "${_FF_CMD_DEFINE_RET_INITS} retVar${k}(retVar${k})")
                set(_FF_CMD_DEFINE_RET_ARGS "${_FF_CMD_DEFINE_RET_ARGS} retType${k} const& retVar${k}")
            endforeach()
        endif()

        string(STRIP "${_FF_CMD_DEFINE_MACRO_INITS}" _FF_CMD_DEFINE_MACRO_INITS)
        string(STRIP "${_FF_CMD_DEFINE_MACRO_ARGS}" _FF_CMD_DEFINE_MACRO_ARGS)
        string(STRIP "${_FF_CMD_DEFINE_RET_ARGS}" _FF_CMD_DEFINE_RET_ARGS)

        if(r GREATER 0)
            if(i GREATER 0)
            set(_FF_CMD_DEFINE_MACRO "${_FF_CMD_DEFINE_MACRO}, ret_format_str, ret_format_args) \\\n\
struct name##Ret : public ff::CmdRet { \\\n\
${_FF_CMD_DEFINE_RET_MEMBERS}\\\n\
name##Ret(${_FF_CMD_DEFINE_RET_ARGS}) : ${_FF_CMD_DEFINE_RET_INITS} {} \\\n\
_FF_CMD_DEFINE_STRINGIFY(ret_format_str, ret_format_args) \\\n\
}; \\\n\
struct name final : public ff::Cmd<name##Ret> { \\\n\
${_FF_CMD_DEFINE_MACRO_MEMBERS}\\\n\
name(${_FF_CMD_DEFINE_MACRO_ARGS}) : ${_FF_CMD_DEFINE_MACRO_INITS} {} \\\n\
_FF_CMD_DEFINE_STRINGIFY(format_str, format_args) \\\n\
void serialize(ff::Serializer& serializer) override { \\\n\
${_FF_CMD_DEFINE_MACRO_SERIALIZERS} \\\n\
} \\\n\
static char const* getCmdName() { \\\n\
    return cmd_str; \\\n\
} \\\n\
std::string getName() const override { \\\n\
    return getCmdName(); \\\n\
}\\\n\
static std::string getDescription() { \\\n\
    return desc_str; \\\n\
} \\\n\
name() {} \\\n\
};\\\n\
FF_CMD_REGISTER(name)")
            else()
            set(_FF_CMD_DEFINE_MACRO "${_FF_CMD_DEFINE_MACRO}, ret_format_str, ret_format_args) \\\n\
struct name##Ret : public ff::CmdRet { \\\n\
${_FF_CMD_DEFINE_RET_MEMBERS}\\\n\
name##Ret(${_FF_CMD_DEFINE_RET_ARGS}) : ${_FF_CMD_DEFINE_RET_INITS} {} \\\n\
_FF_CMD_DEFINE_STRINGIFY(ret_format_str, ret_format_args) \\\n\
}; \\\n\
struct name final : public ff::Cmd<name##Ret> { \\\n\
${_FF_CMD_DEFINE_MACRO_MEMBERS}\\\n\
name(${_FF_CMD_DEFINE_MACRO_ARGS}) {} \\\n\
std::string stringify() const override { return \"\"; } \\\n\
void serialize(ff::Serializer& serializer) override { \\\n\
${_FF_CMD_DEFINE_MACRO_SERIALIZERS} \\\n\
} \\\n\
static char const* getCmdName() { \\\n\
    return cmd_str; \\\n\
} \\\n\
std::string getName() const override { \\\n\
    return getCmdName(); \\\n\
}\\\n\
static std::string getDescription() { \\\n\
    return desc_str; \\\n\
} \\\n\
};\\\n\
FF_CMD_REGISTER(name)")
            endif()
        else()
            if(i GREATER 0)
            set(_FF_CMD_DEFINE_MACRO "${_FF_CMD_DEFINE_MACRO}) \\\n\
struct name##Ret : public ff::CmdRet { \\\n\
    std::string stringify() const override { return \"\"; } \\\n\
}; \\\n\
struct name final : public ff::Cmd<name##Ret> { \\\n\
${_FF_CMD_DEFINE_MACRO_MEMBERS}\\\n\
name(${_FF_CMD_DEFINE_MACRO_ARGS}) : ${_FF_CMD_DEFINE_MACRO_INITS} {} \\\n\
_FF_CMD_DEFINE_STRINGIFY(format_str, format_args) \\\n\
void serialize(ff::Serializer& serializer) override { \\\n\
${_FF_CMD_DEFINE_MACRO_SERIALIZERS} \\\n\
} \\\n\
static char const* getCmdName() { \\\n\
    return cmd_str; \\\n\
} \\\n\
std::string getName() const override { \\\n\
    return getCmdName(); \\\n\
} \\\n\
static std::string getDescription() { \\\n\
    return desc_str; \\\n\
} \\\n\
name() {} \\\n\
};\\\n\
FF_CMD_REGISTER(name)")
            else()
            set(_FF_CMD_DEFINE_MACRO "${_FF_CMD_DEFINE_MACRO}) \\\n\
struct name##Ret : public ff::CmdRet { \\\n\
    std::string stringify() const override { return \"\"; } \\\n\
}; \\\n\
struct name final : public ff::Cmd<name##Ret> { \\\n\
${_FF_CMD_DEFINE_MACRO_MEMBERS}\\\n\
name(${_FF_CMD_DEFINE_MACRO_ARGS}) {} \\\n\
std::string stringify() const override { return \"\"; } \\\n\
void serialize(ff::Serializer& serializer) override { \\\n\
${_FF_CMD_DEFINE_MACRO_SERIALIZERS} \\\n\
} \\\n\
static char const* getCmdName() { \\\n\
    return cmd_str; \\\n\
} \\\n\
std::string getName() const override { \\\n\
    return getCmdName(); \\\n\
} \\\n\
static std::string getDescription() { \\\n\
    return desc_str; \\\n\
} \\\n\
};\\\n\
FF_CMD_REGISTER(name)")
            endif()
        endif()

        set(FF_CMD_DEFINE_MACROS "${FF_CMD_DEFINE_MACROS}${_FF_CMD_DEFINE_MACRO}\n")
    endforeach()
endforeach()

configure_file(include/ff/messages/CmdHelpers.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/ff/messages/CmdHelpers.hpp)

