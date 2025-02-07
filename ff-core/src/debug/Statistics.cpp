/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/debug/Statistics.hpp>

#include <ff/Console.hpp>
#include <ff/CVars.hpp>

namespace ff {
    Statistics::Statistics() {
    }
    Statistics::~Statistics() {
    }
    
    void Statistics::beginStopwatch(const std::string& name) {
        auto it = _stopwatches.find(name);
        if(it == _stopwatches.end()) {
            _stopwatches.insert(std::make_pair(name, StopwatchStatistic{}));
            it = _stopwatches.find(name);
            it->second.lastDuration = 0;
        }
        it->second.measuring = true;
        it->second.beginTick = timer_current();
    }
    float Statistics::endStopwatch(const std::string& name) {
        auto it = _stopwatches.find(name);
        FF_ASSERT(it != _stopwatches.end(), "`%s` is not a statistic.", name);
        it->second.measuring = false;
        // http://maniccoder.blogspot.com/2011/03/timing.html
        it->second.lastDuration = timer_elapsed(it->second.beginTick);
        return it->second.lastDuration;
    }
    bool Statistics::doesStopwatchExist(const std::string& name) const {
        return _stopwatches.find(name) != _stopwatches.end();
    }
    float Statistics::getStopwatchValue(const std::string& name) const {
        auto it = _stopwatches.find(name);
        FF_ASSERT(it != _stopwatches.end(), "`%s` is not a valid stopwatch.", name);
        return it->second.lastDuration;
    }
    void Statistics::printStopwatchValue(const std::string& name) const {
        FF_CONSOLE_LOG("Value of stopwatch `%s`: %s", name, getStopwatchValue(name));
    }

    void Statistics::pushListValue(const std::string& name, float value) {
        auto it = _lists.find(name);
        if(it == _lists.end()) {
            _lists.insert(std::make_pair(name, ListStatistic{}));
            it = _lists.find(name);
            it->second.maxSize = CVars::get<int>("debug_statistics_list_max_size");
        }
        ListStatistic& list = it->second;
        list.values.push_back(value);
        if(list.maxSize > 0
            && list.values.size() > list.maxSize) {
            list.values.erase(list.values.begin());
        }
    }
    bool Statistics::doesListExist(const std::string& name) const {
        return _lists.find(name) != _lists.end();
    }
    void Statistics::clearList(const std::string& name) {
        auto it = _lists.find(name);
        FF_ASSERT(it != _lists.end(), "`%s` is not a valid list.", name);
        it->second.values.clear();
    }
    void Statistics::printList(const std::string& name) const {
        auto it = _lists.find(name);
        FF_ASSERT(it != _lists.end(), "`%s` is not a valid list.", name);
        FF_CONSOLE_LOG("Values of list `%s`:", name);
        for(auto& value : it->second.values) {
            FF_CONSOLE_LOG("%s", value);
        }
    }
    void Statistics::setListMaxSize(const std::string& name, int const& maxSize) {
        auto it = _lists.find(name);
        FF_ASSERT(it != _lists.end(), "`%s` is not a valid list.", name);
        it->second.maxSize = maxSize;
        if(it->second.values.size() > it->second.maxSize) {
            it->second.values.erase(it->second.values.begin(),
                it->second.values.begin() + (it->second.values.size() - it->second.maxSize));
        }
    }
    float Statistics::getListTopValue(const std::string& name) const {
        auto it = _lists.find(name);
        FF_ASSERT(it != _lists.end(), "`%s` is not a valid list.", name);
        if(it->second.values.size() == 0) {
            return 0;
        }
        return it->second.values[it->second.values.size() - 1];
    }
    float Statistics::getListAverage(const std::string& name) const {
        auto it = _lists.find(name);
        FF_ASSERT(it != _lists.end(), "`%s` is not a valid list.", name);
        if(it->second.values.size() == 0) {
            return 0;
        }
        float n = 0;
        for(int i = 0; i < it->second.values.size(); i++) {
            n += it->second.values[i];
        }
        return n / it->second.values.size();
    }
    float Statistics::getListMedian(const std::string& name) const {
        auto it = _lists.find(name);
        FF_ASSERT(it != _lists.end(), "`%s` is not a valid list.", name);
        if(it->second.values.size() == 0) {
            return 0;
        }

        int const size = it->second.values.size();
        _medianScratchpad.clear();
        _medianScratchpad.reserve(size);
        _medianScratchpad.insert(_medianScratchpad.end(),
            it->second.values.begin(),
            it->second.values.end());
        std::sort(_medianScratchpad.begin(), _medianScratchpad.end());

        if(_medianScratchpad.size() % 2 == 0) {
            return (_medianScratchpad[size / 2 - 1] + _medianScratchpad[size / 2]) / 2.0f;
        }
        return _medianScratchpad[size / 2];
    }
    int Statistics::getListCount() const {
        return _lists.size();
    }
    void Statistics::listForEach(std::function<void(std::string)> l) {
        for(auto const& pair : _lists) {
            l(pair.first);
        }
    }
}
