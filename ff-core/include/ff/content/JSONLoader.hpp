/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// @todo Re-implement using AssetBundle

// #ifndef _FAITHFUL_FOUNTAIN_CONTENT_JSON_LOADER_HPP
// #define _FAITHFUL_FOUNTAIN_CONTENT_JSON_LOADER_HPP

// #include <ff/content/ContentManager.hpp>
// #include <nlohmann/json.hpp>
// #include <ff/util/Macros.hpp>

// namespace ff {
//     template<>
//     struct DefaultContentLoader<nlohmann::json> {
//         inline static std::shared_ptr<nlohmann::json> load(ContentManager& contentManager, std::istream& stream) {
//             FF_UNUSED(contentManager);

//             std::string str(std::istreambuf_iterator<char>(stream), {});
//             return std::make_shared<nlohmann::json>(nlohmann::json::parse(str));
//         }
//     };
// }

// #endif
