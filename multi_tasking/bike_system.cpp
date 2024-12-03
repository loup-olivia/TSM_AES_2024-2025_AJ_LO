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
 * @file bike_system.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Bike System implementation (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "bike_system.hpp"

#include <chrono>

#include "Callback.h"
#include "gear_device.hpp"
#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_tasking {

static constexpr std::chrono::milliseconds kGearTaskPeriod                   = 800ms;
static constexpr std::chrono::milliseconds kGearTaskDelay                    = 0ms;
static constexpr std::chrono::milliseconds kGearTaskComputationTime          = 100ms;
static constexpr std::chrono::milliseconds kSpeedDistanceTaskPeriod          = 400ms;
static constexpr std::chrono::milliseconds kSpeedDistanceTaskDelay           = 0ms;
static constexpr std::chrono::milliseconds kSpeedDistanceTaskComputationTime = 200ms;
static constexpr std::chrono::milliseconds kDisplayTaskPeriod               = 1600ms;
static constexpr std::chrono::milliseconds kDisplayTaskDelay                = 300ms;
static constexpr std::chrono::milliseconds kDisplayTaskComputationTime      = 500ms;
static constexpr std::chrono::milliseconds kResetTaskPeriod                  = 800ms;
static constexpr std::chrono::milliseconds kResetTaskDelay                   = 700ms;
static constexpr std::chrono::milliseconds kResetTaskComputationTime         = 100ms;
static constexpr std::chrono::milliseconds kTemperatureTaskPeriod            = 1600ms;
static constexpr std::chrono::milliseconds kTemperatureTaskDelay             = 1100ms;
static constexpr std::chrono::milliseconds kTemperatureTaskComputationTime   = 100ms;
static constexpr std::chrono::milliseconds kMajorCycleDuration               = 1600ms;

BikeSystem::BikeSystem()
    : _timer(),
      _eventQueue(),
      _gearDevice(_eventQueue, callback(this, &BikeSystem::onGearChange)),
      _pedalDevice(_eventQueue, callback(this, &BikeSystem::onSpeedChange)),
      _resetDevice(callback(this, &BikeSystem::onReset)),
      _speedometer(_timer),
      _displayDevice(),
      _taskLogger(),
      _cpuLogger(_timer) {}

      //ajouter le memorylogger pour faire getAndPrintStatistics()
      //comme dans le codelab multi-tasking ajouter aussi un printDiff()

void BikeSystem::start() {
    tr_info("Starting Super-Loop with event handling");

    init();

    EventQueue eventQueue;  // create the event queue

    auto startTime = _timer.elapsed_time();

    // Schedule the gearEvent task
    Event<void()> gearEvent(
        &eventQueue,
        callback(this, &BikeSystem::gearTask));  // Create the event with callback on the
                                                 // wanted function
    gearEvent.delay(kGearTaskDelay);    // define the delay between two calls of the task
    gearEvent.period(kGearTaskPeriod);  // define the period
    gearEvent.post();                   // schedule the task to the event queue
    
    // Schedule the speedDistance task
    Event<void()> speedDistanceEvent(&eventQueue,
                                     callback(this, &BikeSystem::speedDistanceTask));
    speedDistanceEvent.delay(kSpeedDistanceTaskDelay);
    speedDistanceEvent.period(kSpeedDistanceTaskPeriod);
    speedDistanceEvent.post();

    // Schedule the temperatureTask
    Event<void()> temperatureTaskEvent(&eventQueue,
                                       callback(this, &BikeSystem::temperatureTask));
    temperatureTaskEvent.delay(kTemperatureTaskDelay);
    temperatureTaskEvent.period(kTemperatureTaskPeriod);
    temperatureTaskEvent.post();

    // Schedule the resetTask
    Event<void()> resetTaskEvent(&eventQueue, callback(this, &BikeSystem::resetTask));
    resetTaskEvent.delay(kResetTaskDelay);
    resetTaskEvent.period(kResetTaskPeriod);
    resetTaskEvent.post();

    // Schedule the displayTask1
    Event<void()> displayTaskEvent(&eventQueue,
                                    callback(this, &BikeSystem::displayTask));
    displayTaskEvent.delay(kDisplayTaskDelay);
    displayTaskEvent.period(kDisplayTaskPeriod);
    displayTaskEvent.post();


    tr_info("All tasks posted");

#if !MBED_TEST_MODE
    Event<void()> printStatsEvent(
        &eventQueue, callback(&_cpuLogger, &advembsof::CPULogger::printStats));
    printStatsEvent.delay(kMajorCycleDuration);
    printStatsEvent.period(kMajorCycleDuration);
    printStatsEvent.post();
#endif

    eventQueue.dispatch_forever();
}

void BikeSystem::stop() { 
    //ajout d'un break dispatch à faire
    core_util_atomic_store_bool(&_stopFlag, true); 
}

#if defined(MBED_TEST_MODE)
const advembsof::TaskLogger& BikeSystem::getTaskLogger() { return _taskLogger; }
#endif  // defined(MBED_TEST_MODE)

void BikeSystem::init() {
    // start the timer
    _timer.start();

    // initialize the lcd display
    disco::ReturnCode rc = _displayDevice.init();
    if (rc != disco::ReturnCode::Ok) {
        tr_error("Failed to initialized the lcd display: %d", static_cast<int>(rc));
    }

    // initialize the sensor device
    bool present = _sensorDevice.init();
    if (!present) {
        tr_error("Sensor not present or initialization failed");
    }

    // enable/disable task logging
    _taskLogger.enable(true);
}

void BikeSystem::gearTask() {
    // gear task
    auto taskStartTime = _timer.elapsed_time();

    // no need to protect access to data members (single threaded)
    _currentGear     = _gearDevice.getCurrentGear();
    _currentGearSize = _gearDevice.getCurrentGearSize();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kGearTaskIndex, taskStartTime);
}

void BikeSystem::onGearChange(){

}

void BikeSystem::speedDistanceTask() {
    // speed and distance task
    auto taskStartTime = _timer.elapsed_time();

    const auto pedalRotationTime = _pedalDevice.getCurrentRotationTime();
    _speedometer.setCurrentRotationTime(pedalRotationTime);
    _speedometer.setGearSize(_currentGearSize);
    // no need to protect access to data members (single threaded)
    _currentSpeed     = _speedometer.getCurrentSpeed();
    _traveledDistance = _speedometer.getDistance();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kSpeedTaskIndex, taskStartTime);
}

void BikeSystem::temperatureTask() {
    auto taskStartTime = _timer.elapsed_time();

    // no need to protect access to data members (single threaded)
    _currentTemperature = _sensorDevice.readTemperature();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kTemperatureTaskIndex, taskStartTime);
}

void BikeSystem::onReset() {
    _resetTime = _timer.elapsed_time();
    core_util_atomic_store_bool(&_resetFlag, true);
}

void BikeSystem::resetTask() {
    auto taskStartTime = _timer.elapsed_time();

    if (core_util_atomic_load_bool(&_resetFlag)) {
        tr_info("Reset task: response time is %" PRIu64 " usecs",
                (_timer.elapsed_time() - _resetTime).count());

        core_util_atomic_store_bool(&_resetFlag, false);
        _speedometer.reset();
    }

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kResetTaskIndex, taskStartTime);
}

void BikeSystem::displayTask() {
    auto taskStartTime = _timer.elapsed_time();

    _displayDevice.displayGear(_currentGear);
    _displayDevice.displaySpeed(_currentSpeed);
    _displayDevice.displayDistance(_traveledDistance);
    _displayDevice.displayTemperature(_currentTemperature);

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kDisplayTask1Index, taskStartTime);
}
}  // namespace static_scheduling_with_event