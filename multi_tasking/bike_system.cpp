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
#include <cstdint>

#include "Callback.h"
#include "gear_device.hpp"
#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_tasking {

static constexpr std::chrono::milliseconds kDisplayTaskPeriod               = 1600ms;
static constexpr std::chrono::milliseconds kDisplayTaskDelay                = 300ms;
static constexpr std::chrono::milliseconds kDisplayTaskComputationTime      = 500ms;
static constexpr std::chrono::milliseconds kTemperatureTaskPeriod            = 1600ms;
static constexpr std::chrono::milliseconds kTemperatureTaskDelay             = 1100ms;
static constexpr std::chrono::milliseconds kTemperatureTaskComputationTime   = 100ms;
static constexpr std::chrono::milliseconds kMajorCycleDuration               = 1600ms;

BikeSystem::BikeSystem()
    : _timer(),
      _ThreadISR(osPriorityBelowNormal, OS_STACK_SIZE, nullptr, "ISRThread"),
      _eventQueuePeriodic(),
      _eventQueueISR(),
      _gearDevice(_eventQueuePeriodic, callback(this, &BikeSystem::onGearEvent)),
      _pedalDevice(_eventQueuePeriodic, callback(this, &BikeSystem::onPedalEvent)),
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


    auto startTime = _timer.elapsed_time();               // schedule the task to the event queue
    
    // Schedule the temperatureTask
    Event<void()> temperatureTaskEvent(&_eventQueuePeriodic,
                                       callback(this, &BikeSystem::temperatureTask));
    temperatureTaskEvent.delay(kTemperatureTaskDelay);
    temperatureTaskEvent.period(kTemperatureTaskPeriod);
    temperatureTaskEvent.post();


    // Schedule the displayTask
    Event<void()> displayTaskEvent(&_eventQueuePeriodic,
                                    callback(this, &BikeSystem::displayTask));
    displayTaskEvent.delay(kDisplayTaskDelay);
    displayTaskEvent.period(kDisplayTaskPeriod);
    displayTaskEvent.post();

        // Memory logger task
    Event<void()> memoryLoggerEvent(
        &_eventQueuePeriodic, callback(&_memoryLogger, &advembsof::MemoryLogger::printDiffs));
    memoryLoggerEvent.period(kMajorCycleDuration);
    memoryLoggerEvent.delay(kMajorCycleDuration);
    memoryLoggerEvent.post();

    tr_info("All tasks posted");

#if !MBED_TEST_MODE
    Event<void()> printStatsEvent(
        &_eventQueuePeriodic, callback(&_cpuLogger, &advembsof::CPULogger::printStats));
    printStatsEvent.delay(kMajorCycleDuration);
    printStatsEvent.period(kMajorCycleDuration);
    printStatsEvent.post();
#endif
    _ThreadISR.start(callback(&_eventQueueISR, &EventQueue::dispatch_forever));

    _memoryLogger.getAndPrintStatistics();
    _memoryLogger.printDiffs();

    _eventQueuePeriodic.dispatch_forever();
    //code never gets bellow

}

void BikeSystem::stop() { 
    core_util_atomic_store_bool(&_stopFlag, true); 
    _eventQueuePeriodic.break_dispatch();
}

#if defined(MBED_TEST_MODE)
//const advembsof::TaskLogger& BikeSystem::getTaskLogger() { return _taskLogger; }
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


void BikeSystem::onGearEvent(uint8_t gear, uint8_t gearSize){
    _currentGear = gear;
    _speedometer.setGearSize(gearSize);
    
}

void BikeSystem::onPedalEvent(const std::chrono::milliseconds& rotationTime){
    _speedometer.setCurrentRotationTime(rotationTime);
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
    _eventQueueISR.call(callback(this, &BikeSystem::resetTask));
}

void BikeSystem::resetTask() {
      //tr_info("Reset task: response time is %" PRIu64 " usecs",
      //          (_timer.elapsed_time() - _resetTime).count());
    _speedometer.reset();
}

void BikeSystem::displayTask() {

    _currentSpeed = _speedometer.getCurrentSpeed();
    _traveledDistance = _speedometer.getDistance();

    auto taskStartTime = _timer.elapsed_time();
    _displayDevice.displayGear(_currentGear);
    _displayDevice.displaySpeed(_currentSpeed);
    _displayDevice.displayDistance(_traveledDistance);
    _displayDevice.displayTemperature(_currentTemperature);

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kDisplayTask1Index, taskStartTime);
}
}  // namespace static_scheduling_with_event
