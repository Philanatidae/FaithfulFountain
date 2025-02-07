/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/ClearColor.hpp>

namespace ff {

ClearColor::ClearColor(const double& r, const double& g, const double& b, const double& a)
	:r(r), g(g), b(b), a(a) {
}

ClearColor ClearColor::identity() {
	return ClearColor(1, 1, 1, 1);
}
ClearColor ClearColor::white() {
	return ClearColor(1, 1, 1, 1);
}
ClearColor ClearColor::black() {
	return ClearColor(0, 0, 0, 1);
}
ClearColor ClearColor::transparentBlack() {
	return ClearColor(0, 0, 0, 0);
}
ClearColor ClearColor::transparentWhite() {
	return ClearColor(1, 1, 1, 0);
}
ClearColor ClearColor::red() {
	return ClearColor(1, 0, 0, 1);
}
ClearColor ClearColor::green() {
	return ClearColor(0, 1, 0, 1);
}
ClearColor ClearColor::blue() {
	return ClearColor(0, 0, 1, 1);
}
ClearColor ClearColor::yellow() {
	return ClearColor(1, 1, 0, 1);
}
ClearColor ClearColor::cyan() {
	return ClearColor(0, 1, 1, 1);
}
ClearColor ClearColor::magenta() {
	return ClearColor(1, 0, 1, 1);
}

double* ClearColor::data() {
	return &r;
}
double const* ClearColor::data() const {
	return &r;
}

}
