/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#version 450

layout(location=0) out uvec2 FragColor;

layout(location=0) in flat uvec2 io_actorID;

void main() {
     FragColor = io_actorID;
}
