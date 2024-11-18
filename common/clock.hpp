// Copyright 2022 Haute école d'ingénierie et d'architecture de Fribourg
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/****************************************************************************
 * @file clock.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Clock class used for demonstrating race condition/critical section
 * @date 2022-09-01
 * @version 0.1.0
 ***************************************************************************/

#pragma once

#include "mbed.h"
#include "mstd_mutex"
#include "stdio.h"

namespace multi_tasking {

class Clock {
public:
    struct DateTimeType {
        uint32_t day;
        uint32_t hour;
        uint32_t minute;
        uint32_t second;
    };

    Clock() = default;

    void start() {
        // start a ticker thread for dispatching events that are queued in the tickerUpdate() method
        _tickerThread.start(callback(&_tickerQueue, &EventQueue::dispatch_forever));
        // call the tickerUpdate() method every second, for queueing an event to be dispatched by the ticker thread
        _ticker.attach(callback(this, &Clock::tickerUpdate), clockUpdateTimeout);
        //modified version to execute from ISR -> return an error
        //_ticker.attach(callback(this, &Clock::updateCurrentTime), clockUpdateTimeout);

        // schedule an event every second for displaying the time on the console
        _clockDisplayQueue.call_every(clockDisplayTimeout, 
                                      callback(this, &Clock::getAndPrintDateTime));
        // dispatch events from the thread calling the start() method (main thread)
        _clockDisplayQueue.dispatch_forever();
    }

private:
    void getAndPrintDateTime() {
        //with wait
        /*DateTimeType dt = {0};

        dt.day = _currentTime.day;
        dt.hour = _currentTime.hour;

        static constexpr std::chrono::microseconds waitTime = 1s;
        wait_us(waitTime.count());

        dt.minute = _currentTime.minute;  
        dt.second = _currentTime.second;

        printf("Day %d Hour %d min %d sec %d\n", dt.day, dt.hour, dt.minute, dt.second);*/

        //Without wait

        DateTimeType dt = {0};

        mstd::lock_guard<mstd::mutex> lock(_timeMutex); //implementing mutex for shared ressources
        dt.day = _currentTime.day;
        dt.hour = _currentTime.hour;

        dt.minute = _currentTime.minute;
        dt.second = _currentTime.second;
 
 
        printf("Day %u Hour %u min %u sec %u\n", dt.day, dt.hour, dt.minute, dt.second);
        
    }

    void tickerUpdate() {
        _tickerQueue.call(callback(this, &Clock::updateCurrentTime));
    }

    void updateCurrentTime() {
        mstd::lock_guard<mstd::mutex> lock(_timeMutex); //implementing mutex for shared ressources
        _currentTime.second += std::chrono::duration_cast<std::chrono::seconds>(
          clockUpdateTimeout).count();

        if (_currentTime.second > 59) {
            _currentTime.second = 0;
            _currentTime.minute++;
            if (_currentTime.minute > 59) {
                _currentTime.minute = 0;
                _currentTime.hour++;
                if (_currentTime.hour > 23) {
                    _currentTime.hour = 0;
                    _currentTime.day++;
                }
            }
        }
    }

    EventQueue _clockDisplayQueue;
    Ticker _ticker;
    EventQueue _tickerQueue;
    Thread _tickerThread;
    DateTimeType _currentTime{ .day = 0, .hour = 10, .minute = 59, .second = 59};
    static constexpr std::chrono::milliseconds clockUpdateTimeout = 1000ms;
    static constexpr std::chrono::milliseconds clockDisplayTimeout = 1000ms;

    //mutex 
    mstd::mutex _timeMutex;
};

} // namespace multi_tasking