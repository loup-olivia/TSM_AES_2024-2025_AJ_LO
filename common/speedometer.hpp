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
 * @file speedometer_device.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief WheelCounterDevice header file (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "constants.hpp"
#include "mbed.h"

namespace bike_computer {

class Speedometer {
   public:
    explicit Speedometer(Timer& timer);  // NOLINT(runtime/references)

    // method used for setting the current pedal rotation time
    void setCurrentRotationTime(const std::chrono::milliseconds& currentRotationTime);

    // method used for setting/getting the current gear
    void setGearSize(uint8_t gearSize);

    // method called for getting the current speed (expressed in km / h)
    float getCurrentSpeed() const;

    // method called for getting the current traveled distance (expressed in km)
    float getDistance();

    // method called for resetting the traveled distance
    void reset();

    // methods used for tests only
#if defined(MBED_TEST_MODE)
    uint8_t getGearSize() const;
    float getWheelCircumference() const;
    float getTraySize() const;
    std::chrono::milliseconds getCurrentPedalRotationTime() const;
    void setOnResetCallback(mbed::Callback<void()> callback);
#endif  // defined(MBED_TEST_MODE)

   private:
    // private methods
    void computeSpeed();
    void computeDistance();

    // definition of task period time
    static constexpr std::chrono::milliseconds kTaskPeriod = 400ms;
    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 200000us;

    // constants related to speed computation
    static constexpr float kWheelCircumference   = 2.1f;
    static constexpr uint8_t kTraySize           = 50;
    std::chrono::microseconds _lastTime          = std::chrono::microseconds::zero();
    std::chrono::milliseconds _pedalRotationTime = kInitialPedalRotationTime;

    // data members
    Timer& _timer;
    LowPowerTicker _ticker;
    float _currentSpeed = 0.0f;
    Mutex _totalDistanceMutex;
    float _totalDistance = 0.0f;
    uint8_t _gearSize    = 1;

    Thread _thread;

#if defined(MBED_TEST_MODE)
    mbed::Callback<void()> _cb;
#endif
};

}  // namespace bike_computer
