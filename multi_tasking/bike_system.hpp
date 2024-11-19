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
 * @file bike_system.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Bike System header file (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

// from advembsof
#include "Timer.h"
#include "cpu_logger.hpp"
#include "display_device.hpp"
#include "task_logger.hpp"

// from common
#include "sensor_device.hpp"
#include "speedometer.hpp"

// local
#include "gear_device.hpp"
#include "pedal_device.hpp"
#include "reset_device.hpp"

namespace multi_tasking {

class BikeSystem {
   public:
    // constructor
    BikeSystem();

    // make the class non copyable
    BikeSystem(BikeSystem&)            = delete;
    BikeSystem& operator=(BikeSystem&) = delete;

    // method called in main() for starting the system
    void start();

    // method called for stopping the system
    void stop();

#if defined(MBED_TEST_MODE)
    const advembsof::TaskLogger& getTaskLogger();
#endif  // defined(MBED_TEST_MODE)

   private:
    // private methods
    void init();
    void gearTask();
    void speedDistanceTask();
    void temperatureTask();
    void resetTask();
    void displayTask1();
    void displayTask2();

    void onReset();

    // stop flag, used for stopping the super-loop (set in stop())
    bool _stopFlag = false;
    // timer instance used for loggint task time and used by ResetDevice
    Timer _timer;
    // data member that represents the device for manipulating the gear
    GearDevice _gearDevice;
    uint8_t _currentGear     = bike_computer::kMinGear;
    uint8_t _currentGearSize = bike_computer::kMinGearSize;
    // data member that represents the device for manipulating the pedal rotation
    // speed/time
    PedalDevice _pedalDevice;
    float _currentSpeed     = 0.0f;
    float _traveledDistance = 0.0f;
    // data member that represents the device used for resetting
    ResetDevice _resetDevice;
    // data member that represents the device display
    advembsof::DisplayDevice _displayDevice;
    // data member that represents the device for counting wheel rotations
    bike_computer::Speedometer _speedometer;
    // data member that represents the sensor device
    bike_computer::SensorDevice _sensorDevice;
    float _currentTemperature = 0.0f;

    // used for logging task info
    advembsof::TaskLogger _taskLogger;

    // used for logging cpu usage
    advembsof::CPULogger _cpuLogger;

    // used to register the occurence of the reset
    std::chrono::microseconds _resetTime = std::chrono::microseconds::zero();
    volatile bool _resetFlag             = false;
};

}  // namespace static_scheduling_with_event
