# Faithul Fountain

**NOTE:** I am currently in the process of open-sourcing the engine. Do not use until this notice is removed.

Faithful Fountain (FF) is a multi-platform game engine written primarily in C++.

This is not intended to compete with Unity, Godot, etc. This project is mainly for my learning and enjoyment.

Development of FF is driven by needs of a specific project. Features are not added to the engine without being used in an actual project (whether released or not). The main point being, FF is not intended to be developed in isolation.

FF's API is very unstable. Large refactors are quite common. Until the API is shown to be stable, it is expected that projects using FF should fork the engine and customize it according to their needs.

FF is licensed under the Mozilla Public License Version 2.0 unless stated otherwise (some files that don't make sense to be required to remain open are licensed into the public domain). New files are expected to prepend the file with the header (found in LicenseHeaders.txt).

## Engine Design

### Design Principles
There are a few main principles that Faithful Fountain has been developed under:
 - The engine is written alongside a project to ensure that features/refactors are actually useful in the real world.
 - Games are "platform and screen agnostic". This allows games to be flexible enough to be deployed to multiple platforms, multiple aspect ratios, etc.
 - The graphics abstraction's API is similar to Metal. I've found that Metal's API is a nice blend of verbosity for low-level API's while still allowing for OpenGL ES 3.0 support.
    - OpenGL ES 3.0 will be retained as long as I can, but I do understand that this API is very old and I've already ran into some limitations.
 - No hampering the ability for FF to be integrated into proprietary software.
    - The engine is open-source under a permissive license to enforce contributing back upstream, but games themselves can be closed-source. The engine should not restrict this ability (i.e. the engine code should not be written such that it forces a project to release proprietary code that they don't want shared).
 - Don't abstract too early.
    - The long-term goal is for FF to be modular, but since the engine is developped alongside a project, the current non-modular nature hasn't been an issue so far.

### Engine Architecture
FF has the following main pieces:
 - Events
 - Entity-Component-System (called Actors in FF)
 - Processes
 - Scenes
 - CVars

Processes can be added to the main GameServicer's ProcessManager, which are updated every update cycle. Processes can be chained together, for example types of animation or dialog boxes can be chains of processes.

There are four main types of processes:
 - Tasks: timers, animations, etc.
    - Aren't actually called tasks in the source code, though this may change. I haven't decided yet.
 - Systems: Long-lived processes that update entities "passively". This is the same thing as a "system" in the Entity-Component-System model.
 - Directors: Long-lived processes that update entities "actively" when an Event is triggered. They don't update every frame.
 - Cores: Top-level only. They are a combination of Systems and Directors, acting every frame and on events. An example is the AudioCore.

Actors are typically created in Scenes. Scenes are a special type of process that handle transitioning Actors from one scene to another.

CVars are global data, typically used for configuration.

`GameServicer` in ff-core is the entry point into the engine. Each game/project using FF defines a `entry` function, which loads game-specific top-level systems and CVars, and starts a Scene. Start here when looking through the engine.

## Build Requirements
The following utilities are required to be installed:
 - CMake
 - Clang (GCC may also work), or MSVC on Windows
 - Xcode (macOS only)
 - Android Studio (Android only) with NDK
 - Ninja (won't be required in the future)
 - [Task](https://taskfile.dev)
 - [msdg-bmfont-xml](https://github.com/soimy/msdf-bmfont-xml)

## Supported Platforms
The following platforms are supported:
 - macOS
 - iOS/iPadOS
 - Android
 - Windows (OpenGL only)

The following platforms have initial support, but may or may not be continued:
 - visionOS

## Project Setup

TODO.

## License

This codebase is primarily licensed until the Mozilla Public License Version 2.0. See `LICENSE` for more information. All files in this repository are released under this license unless stated otherwise.

This repository contains copies of some third-party software that are licensed differently.

