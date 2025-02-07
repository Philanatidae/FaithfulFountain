/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/CVars.hpp>

#include <ff/Locator.hpp>

#include <unordered_set>

#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>

namespace ff {
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<int8_t>() {
        return CVarCastFlags::INT8;
    }
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<int16_t>() {
        return CVarCastFlags::INT16;
    }
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<int32_t>() {
        return CVarCastFlags::INT32;
    }
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<int64_t>() {
        return CVarCastFlags::INT64;
    }
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<uint8_t>() {
        return CVarCastFlags::UINT8;
    }
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<uint16_t>() {
        return CVarCastFlags::UINT16;
    }
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<uint32_t>() {
        return CVarCastFlags::UINT32;
    }
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<uint64_t>() {
        return CVarCastFlags::UINT64;
    }
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<float>() {
        return CVarCastFlags::FLOAT;
    }
    template<>
    CVarCastFlags_t cvarsTypeToCastFlag<double>() {
        return CVarCastFlags::DOUBLE;
    }

    std::string cvarsCastFlagToString(CVarCastFlags_t const& flag) {
        switch(flag) {
        case CVarCastFlags::INT8:
            return "int8_t";
        case CVarCastFlags::INT16:
            return "int16_t";
        case CVarCastFlags::INT32:
            return "int32_t";
        case CVarCastFlags::INT64:
            return "int64_t";
        case CVarCastFlags::UINT8:
            return "uint8_t";
        case CVarCastFlags::UINT16:
            return "uint16_t";
        case CVarCastFlags::UINT32:
            return "uint32_t";
        case CVarCastFlags::UINT64:
            return "uint64_t";
        case CVarCastFlags::FLOAT:
            return "float";
        case CVarCastFlags::DOUBLE:
            return "double";
        case CVarCastFlags::NON_CASTABLE:
            return "non-castable";
        default:
            return "Unknown";
        }
    }

    //region CVar Definition
    ICVar::ICVar(const std::string& name, const CVarFlags& flags, const std::string& description)
        :_name(name),_flags(flags),_description(description),_initialized(false) {
    }
    ICVar::~ICVar() {
    }

    const std::string& ICVar::getName() const {
        return _name;
    }
    const CVarFlags& ICVar::getFlags() const {
        return _flags;
    }
    const std::string& ICVar::getDescription() const {
        return _description;
    }

    const bool& ICVar::getInitialized() const {
        return _initialized;
    }
    void ICVar::setInitialized(const bool& initialized) {
        _initialized = initialized;
    }

    template<>
    bool CVar<float>::matchesDefault() {
        return glm::abs(_val - _defaultVal) <= 1 / glm::pow(10.0f, INI_FILE_FLOAT_PRECISION);
    }
    template<>
    bool CVar<double>::matchesDefault() {
        return glm::abs(_val - _defaultVal) <= 1 / glm::pow(10.0f, INI_FILE_FLOAT_PRECISION);
    }

    template<>
    std::string CVar<bool>::toString() const {
        return _val ? "true" : "false";
    }
    template<>
    std::string CVar<bool>::defaultToString() const {
        return _defaultVal ? "true" : "false";
    }

    template<>
    void CVar<bool>::drawImGuiEdit() {
        ImGui::Checkbox("##editv", &_val);
    }
    template<>
    void CVar<int8_t>::drawImGuiEdit() {
        int i = _val;
        ImGui::InputInt("##editv", &i);
        _val = (int8_t)glm::clamp(i,
            (int)std::numeric_limits<int8_t>::min(),
            (int)std::numeric_limits<int8_t>::max());
    }
    template<>
    void CVar<uint8_t>::drawImGuiEdit() {
        int i = _val;
        ImGui::InputInt("##editv", &i);
        _val = (uint8_t)glm::clamp(i,
            (int)std::numeric_limits<uint8_t>::min(),
            (int)std::numeric_limits<uint8_t>::max());
    }
    template<>
    void CVar<int16_t>::drawImGuiEdit() {
        int i = _val;
        ImGui::InputInt("##editv", &i);
        _val = (int16_t)glm::clamp(i,
            (int)std::numeric_limits<int16_t>::min(),
            (int)std::numeric_limits<int16_t>::max());
    }
    template<>
    void CVar<uint16_t>::drawImGuiEdit() {
        int i = _val;
        ImGui::InputInt("##editv", &i);
        _val = (uint16_t)glm::clamp(i,
            (int)std::numeric_limits<uint16_t>::min(),
            (int)std::numeric_limits<uint16_t>::max());
    }
    template<>
    void CVar<int32_t>::drawImGuiEdit() {
        int i = _val;
        ImGui::InputInt("##editv", &i);
        _val = (int32_t)glm::clamp(i,
            (int)std::numeric_limits<int32_t>::min(),
            (int)std::numeric_limits<int32_t>::max());
    }
    template<>
    void CVar<uint32_t>::drawImGuiEdit() {
        int i = _val;
        ImGui::InputInt("##editv", &i);
        _val = (uint32_t)glm::clamp(i,
            (int)std::numeric_limits<uint32_t>::min(),
            (int)std::numeric_limits<uint32_t>::max());
    }
    template<>
    void CVar<int64_t>::drawImGuiEdit() {
        int i = _val;
        ImGui::InputInt("##editv", &i);
        _val = (int64_t)glm::clamp(i,
            (int)std::numeric_limits<int64_t>::min(),
            (int)std::numeric_limits<int64_t>::max());
    }
    template<>
    void CVar<uint64_t>::drawImGuiEdit() {
        int i = _val;
        ImGui::InputInt("##editv", &i);
        _val = (uint64_t)glm::clamp(i,
            (int)std::numeric_limits<uint64_t>::min(),
            (int)std::numeric_limits<uint64_t>::max());
    }
    template<>
    void CVar<float>::drawImGuiEdit() {
        ImGui::InputFloat("##editv", &_val);
    }
    template<>
    void CVar<double>::drawImGuiEdit() {
        ImGui::InputDouble("##editv", &_val);
    }
    template<>
    void CVar<std::string>::drawImGuiEdit() {
        ImGui::InputText("##editv",
            &_val,
            0);
    }

    template<>
    CVarCastFlags_t CVar<int8_t>::getCastFlags() const {
        return CVarCastFlags::INT8
            | CVarCastFlags::INT16
            | CVarCastFlags::INT32
            | CVarCastFlags::INT64;
    }
    template<>
    CVarCastFlags_t CVar<int16_t>::getCastFlags() const {
        return CVarCastFlags::INT16
            | CVarCastFlags::INT32
            | CVarCastFlags::INT64;
    }
    template<>
    CVarCastFlags_t CVar<int32_t>::getCastFlags() const {
        return CVarCastFlags::INT32
            | CVarCastFlags::INT64;
    }
    template<>
    CVarCastFlags_t CVar<int64_t>::getCastFlags() const {
        return CVarCastFlags::INT64;
    }
    template<>
    CVarCastFlags_t CVar<uint8_t>::getCastFlags() const {
        return CVarCastFlags::UINT8
            | CVarCastFlags::UINT16
            | CVarCastFlags::UINT32
            | CVarCastFlags::UINT64
            | CVarCastFlags::INT16
            | CVarCastFlags::INT32
            | CVarCastFlags::INT64;
    }
    template<>
    CVarCastFlags_t CVar<uint16_t>::getCastFlags() const {
        return CVarCastFlags::UINT16
            | CVarCastFlags::UINT32
            | CVarCastFlags::UINT64
            | CVarCastFlags::INT32
            | CVarCastFlags::INT64;
    }
    template<>
    CVarCastFlags_t CVar<uint32_t>::getCastFlags() const {
        return CVarCastFlags::UINT32
            | CVarCastFlags::UINT64
            | CVarCastFlags::INT64;
    }
    template<>
    CVarCastFlags_t CVar<uint64_t>::getCastFlags() const {
        return CVarCastFlags::UINT64;
    }
    template<>
    CVarCastFlags_t CVar<float>::getCastFlags() const {
        return CVarCastFlags::FLOAT
            | CVarCastFlags::DOUBLE;
    }
    template<>
    CVarCastFlags_t CVar<double>::getCastFlags() const {
        return CVarCastFlags::DOUBLE;
    }
    //endregion

    ICVar* CVars::getRawCVar(const std::string& name) {
        if(getCVarMap().find(name) == getCVarMap().end()) {
            return nullptr;
        }
        return getCVarMap()[name].get();
    }

    std::vector<std::string> CVars::getCVarNames() {
        std::vector<std::string> names;
        for(auto const& pair : getCVarMap()) {
            names.push_back(pair.first);
        }
        return names;
    }

    void CVars::prepareCVars(const CommandLineOptions& commandLineOptions) {
        std::string configName = CVars::get<std::string>("cvars_config_name");
        std::string defaultsConfigName = CVars::get<std::string>("cvar_defaults_config_name");

        // Grab config paths from the command line. These two are special
        // because they will bootstrap all other CVars.
        for(const auto& pair : commandLineOptions.getCVarNameValueList()) {
            if(pair.first == "cvars_config_name") {
                configName = pair.second;
                FF_CONSOLE_LOG("Changed CVars config name to: %s", configName);
            }
            if(pair.first == "cvar_defaults_config_name") {
                defaultsConfigName = pair.second;
                FF_CONSOLE_LOG("Changed CVar defaults config name to: %s", defaultsConfigName);
            }
        }

        // 1) Update CVar defaults file (detects if defaults were changed, if so
        //  then the actual CVar gets reset). Load the CVar defaults INI file into
        //  the CVar as value (only uninitialized) and compare default.
        std::unordered_set<std::string> cvarDefaultsChanged;
        std::shared_ptr<INIFile> loadedDefaultsINI = loadCVarsINIFromConfig(defaultsConfigName);
        bool needToSaveDefaults = false;
        {
            if(loadedDefaultsINI) {
                setCVarsFromINI(*loadedDefaultsINI, true);

                for(auto& pair : getCVarMap()) {
                    if((pair.second->getFlags() & CVAR_FLAGS_PRESERVE_FLAGS) == CVarFlags::NONE) {
                        // CVar isn't preserved; don't care
                        continue;
                    }
                    if(pair.second->getInitialized()) {
                        // CVar is already initialized. Its value is the real value, not loaded defaults value,
                        // we don't want to modify it.
                        continue;
                    }
                    if(!loadedDefaultsINI->containsKey(pair.first)
                        || !pair.second->matchesDefault()) {
                        // The CVar default value is not in the INI file
                        // OR the default was changed. This CVar default was modified
                        // or will be considered modified.
                        cvarDefaultsChanged.insert(pair.first);
                        needToSaveDefaults = true;
                        FF_CONSOLE_LOG("Default value for `%s` was changed or was not cached; CVar will be reset.", pair.first);
                    }
                }
            } else {
                // Defaults INI does not exist; it needs to be written
                FF_CONSOLE_LOG("CVar defaults config does not exist.");
                needToSaveDefaults = true;
                loadedDefaultsINI = std::make_shared<INIFile>();
            }
        }
        if(needToSaveDefaults) {
            FF_CONSOLE_LOG("Saving CVar defaults...");

            setCVarDefaultsToINI(*loadedDefaultsINI, true);
            saveCVarsINIToConfig(defaultsConfigName, *loadedDefaultsINI);
        }

        // 2) Re-load actual CVars from INI file, then reset CVars that had their
        //  defaults changed, then save CVars back into INI file.
        bool needToSaveCVars = false;
        std::shared_ptr<INIFile> loadedINI = loadCVarsINIFromConfig(configName);
        if(!loadedINI) {
            loadedINI = std::make_shared<INIFile>();
            needToSaveCVars = true;
            FF_CONSOLE_LOG("CVars config does not exist.");
        }
        setCVarsFromINI(*loadedINI, true);

        for(const auto& cvarName : cvarDefaultsChanged) {
            FF_CONSOLE_LOG("Resetting `%s`...", cvarName);
            getCVarMap()[cvarName]->resetToDefault();
            // We've reset at least one, which means we have to save
            needToSaveCVars = true;
        }

        FF_CONSOLE_LOG("Saving CVars...");
        setCVarsToINI(*loadedINI, true);
        saveCVarsINIToConfig(configName, *loadedINI);

        // 3) Load from command line
        FF_CONSOLE_LOG("Loading from command line...");
        loadCVarsFromCommandLineOptions(commandLineOptions);

        // 4) CVars are now initialized
        for(auto& pair : getCVarMap()) {
            pair.second->setInitialized(true);
        }

        FF_CONSOLE_LOG("CVars prepared.");
    }

    INIFile CVars::convertCommandLineOptionsToINI(const CommandLineOptions& commandLineOptions) {
        INIFile ini;
        for (auto it = commandLineOptions.getCVarNameValueList().begin();
             it != commandLineOptions.getCVarNameValueList().end();
             it++) {
            ini.setRawValue(it->first, it->second);
        }
        return ini;
    }
    void CVars::loadCVarsFromCommandLineOptions(const CommandLineOptions& commandLineOptions) {
        INIFile ini = convertCommandLineOptionsToINI(commandLineOptions);
        setCVarsFromINI(ini);
    }

    INIFile CVars::getCVarsAsINI() {
        INIFile ini;
        setCVarsToINI(ini);
        return ini;
    }
    INIFile CVars::getCVarDefaultsAsINI() {
        INIFile ini;
        setCVarDefaultsToINI(ini);
        return ini;
    }
    void CVars::setCVarsToINI(INIFile& ini, const bool& onlyUninitialized) {
        for(const auto& cvar : getCVarMap()) {
            if((cvar.second->getFlags() & CVAR_FLAGS_PRESERVE_FLAGS) == CVarFlags::NONE) {
                continue;
            }
            if(onlyUninitialized
                && cvar.second->getInitialized()) {
                continue;
            }

            cvar.second->setToINI(ini);
        }
    }
    void CVars::setCVarDefaultsToINI(INIFile& ini, const bool& onlyUninitialized) {
        for(const auto& cvar : getCVarMap()) {
            if((cvar.second->getFlags() & CVAR_FLAGS_PRESERVE_FLAGS) == CVarFlags::NONE) {
                continue;
            }
            if(onlyUninitialized
                && cvar.second->getInitialized()) {
                continue;
            }

            cvar.second->setDefaultToINI(ini);
        }
    }
    void CVars::setCVarsFromINI(const INIFile& ini, const bool& onlyUninitialized) {
        for(auto& cvar : getCVarMap()) {
            if((cvar.second->getFlags() & CVAR_FLAGS_READ_ONLY_FLAGS) != CVarFlags::NONE) {
                continue;
            }

            if(ini.containsKey(cvar.first)
                && (!onlyUninitialized
                    || !cvar.second->getInitialized())) {
                cvar.second->setFromINI(ini);
            }
        }
    }

    bool CVars::saveCVarsINIToConfig(const std::string& configPath, INIFile& ini) {
        FF_CONSOLE_LOG("Saving CVars to config `%s`.", configPath);
        std::shared_ptr<ff::BinaryWriter> writer = Locator::getEnvironment().getDataStorage().getConfigWriter(configPath);
        return ini.outputToWriter(*writer);
    }
    std::shared_ptr<INIFile> CVars::loadCVarsINIFromConfig(const std::string& configPath) {
        FF_CONSOLE_LOG("Loading CVars from config `%s`.", configPath);
        if(!Locator::getEnvironment().getDataStorage().doesConfigExist(configPath)) {
            return nullptr;
        }

        std::shared_ptr<BinaryReader> reader = Locator::getEnvironment().getDataStorage().getConfigReader(configPath);
        std::shared_ptr<INIFile> ini = std::make_shared<INIFile>();
        if(ini->populateFromReader(*reader)) {
            return ini;
        }
        return nullptr;
    }

    void CVars::preserveCVars() {
        FF_CONSOLE_LOG("Preserving CVars...");

        INIFile ini = getCVarsAsINI();
        saveCVarsINIToConfig(CVars::get<std::string>("cvars_config_name"), ini);
    }

    std::unordered_map<std::string, std::unique_ptr<ICVar>> & CVars::getCVarMap() {
        static std::unordered_map<std::string, std::unique_ptr<ICVar>> cvarMap;
        return cvarMap;
    }
}
