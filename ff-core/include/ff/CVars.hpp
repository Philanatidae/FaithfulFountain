/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_CVARS_HPP
#define _FAITHFUL_FOUNTAIN_CVARS_HPP

#include <memory>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <ff/Console.hpp>
#include <ff/CommandLineOptions.hpp>
#include <sstream>
#include <set>
#include <iomanip>
#include <ff/util/Macros.hpp>
#include <ff/io/INIFile.hpp>
#include <ff/io/Serializer.hpp>

namespace ff {
    //region Flags
    enum class CVarFlags: uint16_t {
        NONE = 0x0,
        PRESERVE = 0x1,
        DEV_PRESERVE = 0x2,
        READ_ONLY = 0x4,
        HOT_MODIFY = 0x8
    };
    FF_ADD_BITWISE_OPERATORS_TO_ENUM(CVarFlags)

#if FF_DEV_FEATURES
    constexpr CVarFlags CVAR_FLAGS_PRESERVE_FLAGS = CVarFlags::PRESERVE | CVarFlags::DEV_PRESERVE;
    constexpr CVarFlags CVAR_FLAGS_READ_ONLY_FLAGS = CVarFlags::READ_ONLY;
#else
    constexpr CVarFlags CVAR_FLAGS_PRESERVE_FLAGS = CVarFlags::PRESERVE;
    constexpr CVarFlags CVAR_FLAGS_READ_ONLY_FLAGS = CVarFlags::READ_ONLY;
#endif

    using CVarCastFlags_t = uint16_t;
    namespace CVarCastFlags {
        constexpr CVarCastFlags_t INT8 = 1 << 0;
        constexpr CVarCastFlags_t INT16 = 1 << 1;
        constexpr CVarCastFlags_t INT32 = 1 << 2;
        constexpr CVarCastFlags_t INT64 = 1 << 3;
        
        constexpr CVarCastFlags_t UINT8 = 1 << 4;
        constexpr CVarCastFlags_t UINT16 = 1 << 5;
        constexpr CVarCastFlags_t UINT32 = 1 << 6;
        constexpr CVarCastFlags_t UINT64 = 1 << 7;

        constexpr CVarCastFlags_t FLOAT = 1 << 8;
        constexpr CVarCastFlags_t DOUBLE = 1 << 9;

        constexpr CVarCastFlags_t NON_CASTABLE = 1 << 11;
    }

    template<typename T>
    CVarCastFlags_t cvarsTypeToCastFlag() {
        return CVarCastFlags::NON_CASTABLE;
    }
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<int8_t>();
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<int16_t>();
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<int32_t>();
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<int64_t>();
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<uint8_t>();
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<uint16_t>();
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<uint32_t>();
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<uint64_t>();
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<float>();
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<double>();

    std::string cvarsCastFlagToString(CVarCastFlags_t const& flag);
    //endregion

    //region CVar Definition
    class ICVar {
    public:
        ICVar(const std::string& name,
            const CVarFlags& flags = CVarFlags::NONE,
            const std::string& description = "");
        virtual ~ICVar();

        virtual void setToINI(INIFile& iniFile) = 0;
        virtual void setDefaultToINI(INIFile& iniFile) = 0;
        virtual void setFromINI(const INIFile& iniFile) = 0;

        const std::string& getName() const;
        const CVarFlags& getFlags() const;
        const std::string& getDescription() const;

        virtual bool matchesDefault() = 0;
        virtual void resetToDefault() = 0;

        const bool& getInitialized() const;
        void setInitialized(const bool& initialized);

        virtual std::string toString() const = 0;
        virtual std::string defaultToString() const = 0;

        virtual void drawImGuiEdit() = 0;

        virtual CVarCastFlags_t getCastFlags() const = 0;
        virtual CVarCastFlags_t getCastFlagType() const = 0;

        virtual void serialize(Serializer& serializer) = 0;

        template<typename T>
        T castToType() const;

    protected:
        virtual void* getValuePtr() const = 0;

    private:
        std::string _name;
        std::string _description;
        const CVarFlags _flags;
        bool _initialized;

        template<typename T, typename U>
        T castTypeToType() const;
    };

    template<typename T>
    T ICVar::castToType() const {
        switch(getCastFlagType()) {
        case CVarCastFlags::INT8:
            return castTypeToType<T, int8_t>();
        case CVarCastFlags::INT16:
            return castTypeToType<T, int16_t>();
        case CVarCastFlags::INT32:
            return castTypeToType<T, int32_t>();
        case CVarCastFlags::INT64:
            return castTypeToType<T, int64_t>();
        case CVarCastFlags::UINT8:
            return castTypeToType<T, uint8_t>();
        case CVarCastFlags::UINT16:
            return castTypeToType<T, uint16_t>();
        case CVarCastFlags::UINT32:
            return castTypeToType<T, uint32_t>();
        case CVarCastFlags::UINT64:
            return castTypeToType<T, uint64_t>();
        case CVarCastFlags::FLOAT:
            return castTypeToType<T, float>();
        case CVarCastFlags::DOUBLE:
            return castTypeToType<T, double>();
        default:
            FF_CONSOLE_FATAL("Type is non-castable");
        }

        return T();
    }
    template<typename T, typename U>
    T ICVar::castTypeToType() const {
        return (T)*static_cast<U*>(getValuePtr());
    }

    template<typename T>
    class CVar: public ICVar {
    public:
        CVar(const std::string& name, const T& defaultVal, const CVarFlags& flags = CVarFlags::NONE, const std::string& description = "");
        virtual ~CVar();

        T& get();

        T const& getDefault() const;

        void setToINI(INIFile& iniFile) override;
        void setDefaultToINI(INIFile& iniFile) override;
        void setFromINI(const INIFile& iniFile) override;

        virtual bool matchesDefault() override;
        virtual void resetToDefault() override;

        std::string toString() const override;
        std::string defaultToString() const override;

        void drawImGuiEdit() override;

        CVarCastFlags_t getCastFlags() const override;
        CVarCastFlags_t getCastFlagType() const override;

        void* getValuePtr() const override;

        void serialize(Serializer& serializer) override;
    private:
        T _val;
        T const _defaultVal;
    };

    template<typename T>
    CVar<T>::CVar(const std::string& name, const T& defaultVal, const CVarFlags& flags, const std::string& description)
        :ICVar(name, flags, description),_val(defaultVal),_defaultVal(defaultVal) {
    }
    template<typename T>
    CVar<T>::~CVar() {
    }

    template<typename T>
    T& CVar<T>::get() {
        return _val;
    }

    template<typename T>
    T const& CVar<T>::getDefault() const {
        return _defaultVal;
    }
    
    template<typename T>
    void CVar<T>::setToINI(INIFile& iniFile) {
        iniFile.setValue<T>(getName(), _val);
    }
    template<typename T>
    void CVar<T>::setDefaultToINI(INIFile& iniFile) {
        iniFile.setValue<T>(getName(), _defaultVal);
    }
    template<typename T>
    void CVar<T>::setFromINI(const INIFile& iniFile) {
        _val = iniFile.getValue<T>(getName());
    }

    template<typename T>
    bool CVar<T>::matchesDefault() {
        return get() == getDefault();
    }
    template<>
    bool CVar<float>::matchesDefault();
    template<>
    bool CVar<double>::matchesDefault();
    template<typename T>
    void CVar<T>::resetToDefault() {
        _val = _defaultVal;
    }

    template<typename T>
    std::string CVar<T>::toString() const {
        std::ostringstream ss;
        ss << _val;
        return ss.str();
    }
    template<typename T>
    std::string CVar<T>::defaultToString() const {
        std::ostringstream ss;
        ss << _defaultVal;
        return ss.str();
    }

    template<>
    std::string CVar<bool>::toString() const;
    template<>
    std::string CVar<bool>::defaultToString() const;

    template<typename T>
    void CVar<T>::drawImGuiEdit() {
        _val.drawImGuiEdit();
    }
    template<>
    void CVar<bool>::drawImGuiEdit();
    template<>
    void CVar<int8_t>::drawImGuiEdit();
    template<>
    void CVar<uint8_t>::drawImGuiEdit();
    template<>
    void CVar<int16_t>::drawImGuiEdit();
    template<>
    void CVar<uint16_t>::drawImGuiEdit();
    template<>
    void CVar<int32_t>::drawImGuiEdit();
    template<>
    void CVar<uint32_t>::drawImGuiEdit();
    template<>
    void CVar<int64_t>::drawImGuiEdit();
    template<>
    void CVar<uint64_t>::drawImGuiEdit();
    template<>
    void CVar<float>::drawImGuiEdit();
    template<>
    void CVar<double>::drawImGuiEdit();
    template<>
    void CVar<std::string>::drawImGuiEdit();

    template<typename T>
    CVarCastFlags_t CVar<T>::getCastFlags() const {
        // By default, can only cast to self unless
        // defined otherwise (usually this will be
        // NON_CASTABLE).
        return cvarsTypeToCastFlag<T>();
    }
    template<>
    CVarCastFlags_t CVar<int8_t>::getCastFlags() const;
    template<>
    CVarCastFlags_t CVar<int16_t>::getCastFlags() const;
    template<>
    CVarCastFlags_t CVar<int32_t>::getCastFlags() const;
    template<>
    CVarCastFlags_t CVar<int64_t>::getCastFlags() const;
    template<>
    CVarCastFlags_t CVar<uint8_t>::getCastFlags() const;
    template<>
    CVarCastFlags_t CVar<uint16_t>::getCastFlags() const;
    template<>
    CVarCastFlags_t CVar<uint32_t>::getCastFlags() const;
    template<>
    CVarCastFlags_t CVar<uint64_t>::getCastFlags() const;
    template<>
    CVarCastFlags_t CVar<float>::getCastFlags() const;
    template<>
    CVarCastFlags_t CVar<double>::getCastFlags() const;

    template<typename T>
    CVarCastFlags_t CVar<T>::getCastFlagType() const {
        return cvarsTypeToCastFlag<T>();
    }

    template<typename T>
    void* CVar<T>::getValuePtr() const {
        return (void*)&_val;
    }

    template<typename T>
    void CVar<T>::serialize(Serializer& serializer) {
        serializer.serialize(getName().c_str(), _val);
    }
    //endregion

    class CVars {
    public:
        template<typename T>
        static T& create(const std::string& name, const T& defaultVal, const CVarFlags& flags, const std::string& description) {
            getCVarMap().emplace(std::make_pair(name, std::make_unique<CVar<T>>(name, defaultVal, flags, description)));
            return get<T>(name);
        }

        template<typename T>
        static T& get(const std::string& name) {
            auto cvarIt = getCVarMap().find(name);
            if(cvarIt == getCVarMap().end()) {
                FF_CONSOLE_FATAL("Invalid CVar name: " + name);
            }
            // Actually has a slight error; you can technically do get<bool> when
            // it is a string for example. Could use typeid instead if this becomes
            // a big problem.
            FF_ASSERT(cvarIt->second->getCastFlagType() == cvarsTypeToCastFlag<T>(),
                "Types do not match (requested %s, CVar is %s).");
            return static_cast<CVar<T>*>(cvarIt->second.get())->get();
        }
        template<typename T>
        static T cast(const std::string& name) {
            auto cvarIt = getCVarMap().find(name);
            if(cvarIt == getCVarMap().end()) {
                FF_CONSOLE_FATAL("Invalid CVar name: " + name);
            }
            if(cvarIt->second->getCastFlagType() == cvarsTypeToCastFlag<T>()) {
                return static_cast<CVar<T>*>(cvarIt->second.get())->get();
            }
            FF_ASSERT((cvarIt->second->getCastFlags() & cvarsTypeToCastFlag<T>()) > 0,
                "Cannot cast CVar<%s> to CVar<%s>.",
                cvarsCastFlagToString(cvarIt->second->getCastFlagType()),
                cvarsCastFlagToString(cvarsTypeToCastFlag<T>()));
            return cvarIt->second->castToType<T>();
        }

        static ICVar* getRawCVar(const std::string& name);

        static std::vector<std::string> getCVarNames();

        static void prepareCVars(const CommandLineOptions& commandLineOptions);

        static INIFile convertCommandLineOptionsToINI(const CommandLineOptions& commandLineOptions);
        static void loadCVarsFromCommandLineOptions(const CommandLineOptions& commandLineOptions);

        static INIFile getCVarsAsINI();
        static INIFile getCVarDefaultsAsINI();
        static void setCVarsToINI(INIFile& ini, const bool& onlyUninitialized = false);
        static void setCVarDefaultsToINI(INIFile& ini, const bool& onlyUninitialized = false);
        static void setCVarsFromINI(const INIFile& ini, const bool& onlyUninitialized = false);

        static bool saveCVarsINIToConfig(const std::string& configPath, INIFile& ini);
        static std::shared_ptr<INIFile> loadCVarsINIFromConfig(const std::string& configPath);

        static void preserveCVars();

    private:
        static std::unordered_map<std::string, std::unique_ptr<ICVar>>& getCVarMap();
    };
}

//region Macros
#define FF_CVAR_DEFINE(name, type, default_value, flags, help_string) \
    struct name##_FF_CVar { \
        name##_FF_CVar() { \
            ff::CVars::create<type>(#name, default_value, flags, help_string); \
        } \
    }; \
    name##_FF_CVar name##_FF_CVar_val;
//endregion

#endif
