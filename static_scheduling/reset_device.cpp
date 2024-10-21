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
 * @file reset_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief ResetDevice header file (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "reset_device.hpp"

#include <chrono>

#include "PinNames.h"
#include "constants.hpp"
#include "mbed.h"

#if defined(TARGET_DISCO_H747I)
#define PUSH_BUTTON BUTTON1
static constexpr uint8_t kPolarityPressed = 1;
#endif

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "ResetDevice"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling {

static constexpr std::chrono::microseconds kTaskRunTime = 100000us;

ResetDevice::ResetDevice(Timer& timer) : _resetButton(PUSH_BUTTON), _timer(timer) {
    _resetButton.rise(callback(this, &ResetDevice::onRise));
}

bool ResetDevice::checkReset() {
    std::chrono::microseconds initialTime = _timer.elapsed_time();
    std::chrono::microseconds elapsedTime = std::chrono::microseconds::zero();

    while (elapsedTime < kTaskRunTime) {
        if (_resetButton.read() == kPolarityPressed) {
            return true;
        }
        elapsedTime = _timer.elapsed_time() - initialTime;
    }
    return false;
}

void ResetDevice::onRise() { _pressTime = _timer.elapsed_time(); }

std::chrono::microseconds ResetDevice::getPressTime() { return _pressTime; }

}  // namespace static_scheduling
