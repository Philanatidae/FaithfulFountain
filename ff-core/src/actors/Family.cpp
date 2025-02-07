/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/actors/Family.hpp>

#include <ff/actors/Actor.hpp>

namespace ff {
    Family::Family()
        :_allMask(0),_oneMask(0),_excludeMask(0) {
    }
    Family::Family(const Family& other)
        :_allMask(other._allMask),_oneMask(other._oneMask),
        _excludeMask(other._excludeMask) {
    }
    Family::~Family() {
    }

    bool Family::matches(ActorManager* const& actorManager, const Actor_t& actor) const {
        return matches(actorManager->getComponentMask(actor));
    }

    bool Family::equals(const Family& other) const {
        return _allMask == other._allMask
            && _oneMask == other._oneMask
            && _excludeMask == other._excludeMask;
    }
    bool Family::operator==(const Family& rhs) const {
        return equals(rhs);
    }
    bool Family::operator!=(const Family& rhs) const {
        return !equals(rhs);
    }

    bool Family::matches(const ComponentType& mask) const {
        return (mask & _allMask) == _allMask
            && (_oneMask != 0 ? (mask & _oneMask) != 0 : true)
            && (mask & _excludeMask) == 0;
    }

    FamilyBuilder::FamilyBuilder() {
    }
    FamilyBuilder::~FamilyBuilder() {
    }

    Family FamilyBuilder::get() {
        return _family;
    }
}
