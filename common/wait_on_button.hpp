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
 * @file wait_on_button.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief WaitOnButton class used for demonstrating EventFlags
 * @date 2022-09-01
 * @version 0.1.0
 ***************************************************************************/

#pragma once

#include "mbed.h"

#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "WaitOnButton"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_tasking {

#if defined(TARGET_DISCO_H747I)
#define PUSH_BUTTON BUTTON1
#define POLARITY_PRESSED 0
#endif

class WaitOnButton {
public:
    explicit WaitOnButton(const char* threadName) :
        _thread(osPriorityNormal, OS_STACK_SIZE, nullptr, threadName),
        _pushButton(PUSH_BUTTON) 
    {
        _pushButton.fall(callback(this, &WaitOnButton::buttonPressed));
        _timer.start();
        _pressedTime = std::chrono::microseconds::zero();
    }

    void start() {
        osStatus status = _thread.start(callback(this, &WaitOnButton::waitForButtonEvent));
        tr_debug("Thread %s started with status %d", _thread.get_name(), status);
    }

    void wait_started() {
        _eventFlags.wait_any(kStartedEventFlag);
    }

    void wait_exit() {
        _thread.join();
    }

private:
    void waitForButtonEvent() {
        tr_debug("Waiting for button press");
        _eventFlags.set(kStartedEventFlag);
        while (true) {
            _eventFlags.wait_all(kPressedEventFlag);
            std::chrono::microseconds time = _timer.elapsed_time();
            std::chrono::microseconds latency = time - _pressedTime;
            tr_debug("Button pressed with response time: %lld usecs", latency.count());
            tr_debug("Waiting for button press");
        }
    }

    void buttonPressed() {    
        _pressedTime = _timer.elapsed_time();
        _eventFlags.set(kPressedEventFlag);
    }

    static constexpr uint8_t kPressedEventFlag = (1UL << 0);
    static constexpr uint8_t kStartedEventFlag = (1UL << 1);
    Thread _thread;
    Timer _timer;
    std::chrono::microseconds _pressedTime;
    EventFlags _eventFlags;
    InterruptIn _pushButton;
};

} // namespace multi_tasking