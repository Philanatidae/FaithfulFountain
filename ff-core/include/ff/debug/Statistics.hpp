/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_DEBUG_STATISTICS_HPP
#define _FAITHFUL_FOUNTAIN_DEBUG_STATISTICS_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

#include <timer_lib/timer.h>

namespace ff {
    struct StopwatchStatistic {
        double lastDuration;

        tick_t beginTick;
        bool measuring;
    };
    struct ListStatistic {
        std::vector<float> values;
        int maxSize;
    };

    class Statistics {
    public:
        Statistics();
        virtual ~Statistics();
        
        void beginStopwatch(const std::string& name);
        float endStopwatch(const std::string& name);
        bool doesStopwatchExist(const std::string& name) const;
        float getStopwatchValue(const std::string& name) const;
        void printStopwatchValue(const std::string& name) const;

        void pushListValue(const std::string& name, float value);
        bool doesListExist(const std::string& name) const;
        void clearList(const std::string& name);
        void printList(const std::string& name) const;
        void setListMaxSize(const std::string& name, int const& maxSize);
        float getListTopValue(const std::string& name) const;
        float getListAverage(const std::string& name) const;
        float getListMedian(const std::string& name) const;
        int getListCount() const;
        std::string getListName(int const& idx) const;
        void listForEach(std::function<void(std::string)> l);

    private:
        std::unordered_map<std::string, StopwatchStatistic> _stopwatches; 
        std::unordered_map<std::string, ListStatistic> _lists;

        mutable std::vector<float> _medianScratchpad;
    };
}

#endif
