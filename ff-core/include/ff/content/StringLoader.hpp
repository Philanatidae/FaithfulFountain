/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// @todo Re-implement using AssetBundle

// #ifndef _FAITHFUL_FOUNTAIN_CONTENT_STRING_LOADER_HPP
// #define _FAITHFUL_FOUNTAIN_CONTENT_STRING_LOADER_HPP

// #include <string>
// #include <memory>
// #include <iostream>
// #include <ff/content/ContentManager.hpp>

// namespace ff {
//     template<>
//     struct DefaultContentLoader<std::string> {
//         inline static std::shared_ptr<std::string> load(ContentManager& contentManager, std::istream& stream) {
//             FF_UNUSED(contentManager);

//             std::string str(std::istreambuf_iterator<char>(stream), {});
//             return std::make_shared<std::string>(str);
//         }
//     };
// }

// #endif
