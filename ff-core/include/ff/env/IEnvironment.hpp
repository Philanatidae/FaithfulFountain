/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ENV_IENVIRONMENT_HPP
#define _FAITHFUL_FOUNTAIN_ENV_IENVIRONMENT_HPP

#include <string>

#include <ff/env/IDataStorage.hpp>

#include <ff/Console.hpp>

namespace ff {
    using EnvironmentCapabilities_t = uint16_t;
    namespace EnvironmentCapabilities {
        constexpr EnvironmentCapabilities_t MOTION_GRAVITY = 1;
        constexpr EnvironmentCapabilities_t MOTION_ORIENTATION = 1 << 1;
        constexpr EnvironmentCapabilities_t MOTION_ACCELERATION = 1 << 2;
        constexpr EnvironmentCapabilities_t OPEN_URL = 1 << 3;
        constexpr EnvironmentCapabilities_t MOUSE_CURSORS = 1 << 4;
        constexpr EnvironmentCapabilities_t CLIPBOARD = 1 << 5;
        constexpr EnvironmentCapabilities_t SYSTEM_GAMEPAD_ASSIGNMENT = 1 << 6;
    }

    class IEnvironment {
    public:
        IEnvironment() { }
        virtual ~IEnvironment() = default;

        virtual void init() = 0;

        virtual std::string getName() const = 0;
        virtual EnvironmentCapabilities_t getCapabilities() const = 0;

        virtual IDataStorage const& getDataStorage() const = 0;

        virtual void setReportGravity(bool const& enable) = 0;
        virtual void setReportOrientation(bool const& enable) = 0;
        virtual void setReportAcceleration(bool const& enable) = 0;

        virtual bool isReportingGravity() const = 0;
        virtual bool isReportingOrientation() const = 0;
        virtual bool isReportingAcceleration() const = 0;

        virtual std::string getClipboard() const = 0;

        virtual bool isRecordingTextInput() const = 0;
    };

    class NullEnvironment : public IEnvironment {
    public:
        NullEnvironment() { }
        virtual ~NullEnvironment() { }

        void init() override {
        }

        std::string getName() const override {
            return "Null";
        }
        EnvironmentCapabilities_t getCapabilities() const override {
            return 0;
        }

        IDataStorage const& getDataStorage() const override {
            return _dataStorage;
        }

        void setReportGravity(bool const& enable) override {
            FF_CONSOLE_ERROR("Environment does not have gravity motion capability.");
        }
        void setReportOrientation(bool const& enable) override {
            FF_CONSOLE_ERROR("Environment does not have orientation motion capability.");
        }
        void setReportAcceleration(bool const& enable) override {
            FF_CONSOLE_ERROR("Environment does not have acceleration motion capability.");
        }

        bool isReportingGravity() const override {
            return false;
        }
        bool isReportingOrientation() const override {
            return false;
        }
        bool isReportingAcceleration() const override {
            return false;
        }

        std::string getClipboard() const override {
            return "";
        }

        bool isRecordingTextInput() const override {
            return false;
        }

    private:
        NullDataStorage _dataStorage;
    };
}

#endif
