/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMMON_RELOADABLE_HPP
#define _FAITHFUL_FOUNTAIN_COMMON_RELOADABLE_HPP

namespace ff {
    class Reloadable {
    public:
        Reloadable();
        virtual ~Reloadable();

        void reload();

        const bool& getIsValid() const;

    private:
        bool _isValid;

        void invalidate();
        
    protected:
        virtual void onReload() = 0;
        virtual void onInvalidate() = 0;
    };
}

#endif
