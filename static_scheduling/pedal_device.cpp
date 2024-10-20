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
 * @file Pedal_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Pedal Device implementation (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "pedal_device.hpp"

// from disco_h747i/wrappers
#include <chrono>

#include "Timer.h"
#include "joystick.hpp"
#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "PedalDevice"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling {

// definition of task execution time
static constexpr std::chrono::microseconds kTaskRunTime = 100000us;

PedalDevice::PedalDevice(Timer& timer) : _timer(timer){}

std::chrono::milliseconds PedalDevice::getCurrentRotationTime(){
std::chrono::microseconds initialTime = _timer.elapsed_time();
    std::chrono::microseconds elapsedTime = std::chrono::microseconds::zero();
    // we bound the change to one increment/decrement per call
    bool hasChanged = false;
    while (elapsedTime < kTaskRunTime) {
        if (!hasChanged) {
            disco::Joystick::State joystickState =
                disco::Joystick::getInstance().getState();
            switch (joystickState) {
                case disco::Joystick::State::LeftPressed:
                    decreaseRotationSpeed();
                    hasChanged = true;
                    break;
                case disco::Joystick::State::RightPressed:
                    increaseRotationSpeed();
                    hasChanged = true;
                    break;
                default:
                    break;
            }
        }
        elapsedTime = _timer.elapsed_time() - initialTime;
    }
    return _pedalRotationTime;

}

void PedalDevice::increaseRotationSpeed(){
    if (_pedalRotationTime > bike_computer::kMinPedalRotationTime) {
        _pedalRotationTime -= bike_computer::kDeltaPedalRotationTime;
    }
}

void PedalDevice::decreaseRotationSpeed(){
    if (_pedalRotationTime < bike_computer::kMaxPedalRotationTime) {
        _pedalRotationTime += bike_computer::kDeltaPedalRotationTime;
    }
}

}  // namespace static_scheduling