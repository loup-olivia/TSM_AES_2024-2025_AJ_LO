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
 * @file speedometer_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief WheelCounterDevice implementation (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "speedometer.hpp"

#include <chrono>
#include <ratio>

// from disco_h747i/wrappers
#include "joystick.hpp"
#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "Speedometer"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace bike_computer {

Speedometer::Speedometer(Timer& timer) : _timer(timer) {
    // update _lastTime
    _lastTime = _timer.elapsed_time();
}

void Speedometer::setCurrentRotationTime(
    const std::chrono::milliseconds& currentRotationTime) {
    if (_pedalRotationTime != currentRotationTime) {
        // compute distance before changing the rotation time
        computeDistance();

        // change pedal rotation time
        _pedalRotationTime = currentRotationTime;

        // compute speed with the new pedal rotation time
        computeSpeed();
    }
}

void Speedometer::setGearSize(uint8_t gearSize) {
    if (_gearSize != gearSize) {
        // compute distance before chaning the gear size
        computeDistance();

        // change gear size
        _gearSize = gearSize;

        // compute speed with the new gear size
        computeSpeed();
    }
}

float Speedometer::getCurrentSpeed() const { return _currentSpeed; }

float Speedometer::getDistance() {
    // make sure to update the distance traveled
    computeDistance();
    return _totalDistance;
}

void Speedometer::reset() {
    // TODO 

    _totalDistanceMutex.lock();
    _totalDistance = 0.0f;
    _currentSpeed = 0;
    _totalDistanceMutex.unlock();
    _lastTime = _timer.elapsed_time();

}

#if defined(MBED_TEST_MODE)
uint8_t Speedometer::getGearSize() const { return _gearSize; }

float Speedometer::getWheelCircumference() const { return kWheelCircumference; }

float Speedometer::getTraySize() const { return kTraySize; }

std::chrono::milliseconds Speedometer::getCurrentPedalRotationTime() const {
    return _pedalRotationTime;
}

#endif  // defined(MBED_TEST_MODE)

void Speedometer::computeSpeed() {
    // For computing the speed given a rear gear (braquet), one must divide the size of
    // the tray (plateau) by the size of the rear gear (pignon arrière), and then multiply
    // the result by the circumference of the wheel. Example: tray = 50, rear gear = 15.
    // Distance run with one pedal turn (wheel circumference = 2.10 m) = 50/15 * 2.1 m
    // = 6.99m If you ride at 80 pedal turns / min, you run a distance of 6.99 * 80 / min
    // ~= 560 m / min = 33.6 km/h

    // TODO 
    float gearRatio = static_cast<float>(kTraySize)/static_cast<float>(_gearSize);
    float distPerTurn = kWheelCircumference * gearRatio;

    _currentSpeed = distPerTurn * 3600.0f/
    std::chrono::duration_cast<std::chrono::milliseconds>(_pedalRotationTime).count();


}

void Speedometer::computeDistance() {
    // For computing the speed given a rear gear (braquet), one must divide the size of
    // the tray (plateau) by the size of the rear gear (pignon arrière), and then multiply
    // the result by the circumference of the wheel. Example: tray = 50, rear gear = 15.
    // Distance run with one pedal turn (wheel circumference = 2.10 m) = 50/15 * 2.1 m
    // = 6.99m If you ride at 80 pedal turns / min, you run a distance of 6.99 * 80 / min
    // ~= 560 m / min = 33.6 km/h. We then multiply the speed by the time for getting the
    // distance traveled.

    // TODO
    std::chrono::microseconds currentTime = _timer.elapsed_time();
    auto elapsedTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - _lastTime);

    float distance = _currentSpeed * elapsedTime.count() / 3600000.0f;

    // update distance traveled
    _totalDistanceMutex.lock();
    _totalDistance += distance;
    _totalDistanceMutex.unlock();

    // update last time
    _lastTime = currentTime;

}

}  // namespace bike_computer