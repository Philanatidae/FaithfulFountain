# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

target_sources(ff-core PRIVATE
    BufferProcess.cpp
    DelegateProcess.cpp
    IntervalProcess.cpp
    Process.cpp
    ProcessManager.cpp
    ProcessModifier.cpp
    WaitForEventProcess.cpp
    WaitOrKillOnEventProcess.cpp
    WaitProcess.cpp
)
