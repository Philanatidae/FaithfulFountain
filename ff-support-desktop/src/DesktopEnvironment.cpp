/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-support-desktop/DesktopEnvironment.hpp>

#if !defined(__APPLE__)
#define WIN32_LEAN_AND_MEAN
#include <platform_folders.h>
#endif

#include <ff/CVars.hpp>
#include <ff/Console.hpp>
#include <ff/Locator.hpp>

#if defined(FF_HAS_SUPPORT_APPLE)
#include <ff-support-apple/Helpers.hpp>
#endif

namespace ff {
    DesktopEnvironment::DesktopEnvironment(IClipboardProvider* clipboardProvider)
        :_dataStorage(getConfigDirectory()),
        _capabilities(0),
        _clipboardProvider(clipboardProvider) {
    }
    DesktopEnvironment::~DesktopEnvironment() {
    }

    void DesktopEnvironment::init() {
        FF_CONSOLE_LOG("Config directory: `%s`", getConfigDirectory());

        Locator::getMessageBus().addHandler<EnvOpenURLCmd>(this);
        _capabilities |= EnvironmentCapabilities::OPEN_URL;

        Locator::getMessageBus().addHandler<EnvBeginTextInputCmd>(this);
        Locator::getMessageBus().addHandler<EnvEndTextInputCmd>(this);

        _capabilities |= EnvironmentCapabilities::MOUSE_CURSORS;
        _capabilities |= EnvironmentCapabilities::CLIPBOARD;
    }

    std::string DesktopEnvironment::getName() const {
#if defined(__APPLE__)
        return "macOS";
#elif defined(WIN32)
        return "Windows";
#else
        return "Linux";
#endif
    }
    EnvironmentCapabilities_t DesktopEnvironment::getCapabilities() const {
        // @todo Support Windows, Linux motion sensors.
        return _capabilities;
    }

    IDataStorage const& DesktopEnvironment::getDataStorage() const {
        return _dataStorage;
    }

    void DesktopEnvironment::setReportGravity(bool const& enable) {
        FF_CONSOLE_ERROR("Environment does not have gravity motion capability.");
    }
    void DesktopEnvironment::setReportOrientation(bool const& enable) {
        FF_CONSOLE_ERROR("Environment does not have orientation motion capability.");
    }
    void DesktopEnvironment::setReportAcceleration(bool const& enable) {
        FF_CONSOLE_ERROR("Environment does not have acceleration motion capability.");
    }

    bool DesktopEnvironment::isReportingGravity() const {
        return false;
    }
    bool DesktopEnvironment::isReportingOrientation() const {
        return false;
    }
    bool DesktopEnvironment::isReportingAcceleration() const {
        return false;
    }

    std::string DesktopEnvironment::getClipboard() const {
        return _clipboardProvider->getClipboard();
    }

    bool DesktopEnvironment::isRecordingTextInput() const {
        return _isRecordingTextInput;
    }

#ifndef __APPLE__
    // macOS path requires `NSHomeDirectory()`, so it is in DesktopEnvironment.mm
    std::string DesktopEnvironment::getConfigDirectory() const {
        // Windows & Linux are the same; config path + package name
        // @todo Attempt to remove dependency on Sago, at least for Windows
        return (std::filesystem::path(sago::getConfigHome())/CVars::get<std::string>("package_prefix")/CVars::get<std::string>("game_name")).string();
    }
#endif

    std::unique_ptr<typename EnvOpenURLCmd::Ret> DesktopEnvironment::handleCmd(EnvOpenURLCmd const& cmd) {
#if defined(FF_HAS_SUPPORT_APPLE)
        FF_CONSOLE_LOG("Opening `%s`...", cmd.address);
        Helpers::openURL(cmd.address);
#else
        FF_CONSOLE_ERROR("Current environment (%s) does not yet support opening URLs.", getName());
#endif

        return std::make_unique<typename EnvOpenURLCmd::Ret>();
    }
    std::unique_ptr<typename EnvBeginTextInputCmd::Ret> DesktopEnvironment::handleCmd(EnvBeginTextInputCmd const& cmd) {
        _isRecordingTextInput = true;

        return std::make_unique<typename EnvBeginTextInputCmd::Ret>();
    }
    std::unique_ptr<typename EnvEndTextInputCmd::Ret> DesktopEnvironment::handleCmd(EnvEndTextInputCmd const& cmd) {
        return std::make_unique<typename EnvEndTextInputCmd::Ret>();
    }
}
