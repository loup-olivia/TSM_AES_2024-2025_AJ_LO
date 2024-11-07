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

#include "gear_device.hpp"
#include "mbed_trace.h"
#include "rtos.h"
#include "advdembsof_library/utils/cpu_logger.hpp"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling {

static constexpr std::chrono::milliseconds kGearTaskPeriod                   = 800ms;
static constexpr std::chrono::milliseconds kGearTaskDelay                    = 0ms;
static constexpr std::chrono::milliseconds kGearTaskComputationTime          = 100ms;
static constexpr std::chrono::milliseconds kSpeedDistanceTaskPeriod          = 400ms;
static constexpr std::chrono::milliseconds kSpeedDistanceTaskDelay           = 0ms;
static constexpr std::chrono::milliseconds kSpeedDistanceTaskComputationTime = 200ms;
static constexpr std::chrono::milliseconds kDisplayTask1Period               = 1600ms;
static constexpr std::chrono::milliseconds kDisplayTask1Delay                = 300ms;
static constexpr std::chrono::milliseconds kDisplayTask1ComputationTime      = 200ms;
static constexpr std::chrono::milliseconds kResetTaskPeriod                  = 800ms;
static constexpr std::chrono::milliseconds kResetTaskDelay                   = 700ms;
static constexpr std::chrono::milliseconds kResetTaskComputationTime         = 100ms;
static constexpr std::chrono::milliseconds kTemperatureTaskPeriod            = 1600ms;
static constexpr std::chrono::milliseconds kTemperatureTaskDelay             = 1100ms;
static constexpr std::chrono::milliseconds kTemperatureTaskComputationTime   = 100ms;
static constexpr std::chrono::milliseconds kDisplayTask2Period               = 1600ms;
static constexpr std::chrono::milliseconds kDisplayTask2Delay                = 1200ms;
static constexpr std::chrono::milliseconds kDisplayTask2ComputationTime      = 100ms;

BikeSystem::BikeSystem()
    : _timer(),
      _gearDevice(_timer),
      _pedalDevice(_timer),
      _resetDevice(_timer),
      _speedometer(_timer),
      _displayDevice(),
      _sensorDevice(),
      _taskLogger() {}

void BikeSystem::start() {
    tr_info("Starting Super-Loop without event handling");

    //added for cpu logging
    //Timer timer;  // Créer une instance de Timer
    //advembsof::CPULogger _cpuLogger(timer);

    init();

    while (true) {
        auto startTime = _timer.elapsed_time();

        // Calls according to log on the codelab
        gearTask();
        speedDistanceTask();
        displayTask1();
        speedDistanceTask();
        resetTask();
        gearTask();
        speedDistanceTask();
        temperatureTask();
        displayTask2();
        speedDistanceTask();
        resetTask();

        // register the time at the end of the cyclic schedule period and print the
        // elapsed time for the period
        std::chrono::microseconds endTime = _timer.elapsed_time();
        const auto cycle =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        tr_debug("Repeating cycle time is %" PRIu64 " milliseconds", cycle.count());
        bool exit = core_util_atomic_load_bool(&_stopFlag);

        //_cpuLogger.printStats();
        tr_info("cpu-stats");

        if (exit == true) {
            break;
        }
    }
}
// function would be use later

// cppcheck-suppress [unusedFunction, unmatchedSuppression]

void BikeSystem::startWithEventQueue() {
    tr_info("Starting Super-Loop with event queue");

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
    Event<void()> displayTask1Event(&eventQueue,
                                    callback(this, &BikeSystem::displayTask1));
    displayTask1Event.delay(kDisplayTask1Delay);
    displayTask1Event.period(kDisplayTask1Period);
    displayTask1Event.post();

    // Schedule the displayTask2
    Event<void()> displayTask2Event(&eventQueue,
                                    callback(this, &BikeSystem::displayTask2));
    displayTask2Event.delay(kDisplayTask2Delay);
    displayTask2Event.period(kDisplayTask2Period);
    displayTask2Event.post();
    tr_info("All tasks posted");
    eventQueue.dispatch_forever();
}

void BikeSystem::stop() { core_util_atomic_store_bool(&_stopFlag, true); }

#if defined(MBED_TEST_MODE)
// cppcheck-suppress [unusedFunction, unmatchedSuppression]
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

    // simulate task computation by waiting for the required task computation time

    // std::chrono::microseconds elapsedTime = std::chrono::microseconds::zero();
    // while (elapsedTime < kTemperatureTaskComputationTime) {
    //     elapsedTime = _timer.elapsed_time() - taskStartTime;
    // }
    std::chrono::milliseconds elapsedTime = std::chrono::milliseconds::zero();
    ThisThread::sleep_for(kDisplayTask2ComputationTime - elapsedTime);

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kTemperatureTaskIndex, taskStartTime);
}

void BikeSystem::resetTask() {
    auto taskStartTime = _timer.elapsed_time();

    if (_resetDevice.checkReset()) {
        std::chrono::microseconds responseTime =
            _timer.elapsed_time() - _resetDevice.getPressTime();
        tr_info("Reset task: response time is %" PRIu64 " usecs", responseTime.count());
        _speedometer.reset();
    }

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kResetTaskIndex, taskStartTime);
}

void BikeSystem::displayTask1() {
    auto taskStartTime = _timer.elapsed_time();

    _displayDevice.displayGear(_currentGear);
    _displayDevice.displaySpeed(_currentSpeed);
    _displayDevice.displayDistance(_traveledDistance);

    // simulate task computation by waiting for the required task computation time

    // std::chrono::microseconds elapsedTime = std::chrono::microseconds::zero();
    // while (elapsedTime < kDisplayTask1ComputationTime) {
    //     elapsedTime = _timer.elapsed_time() - taskStartTime;
    // }
    std::chrono::milliseconds elapsedTime = std::chrono::milliseconds::zero();
    ThisThread::sleep_for(kDisplayTask2ComputationTime - elapsedTime);

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kDisplayTask1Index, taskStartTime);
}

void BikeSystem::displayTask2() {
    auto taskStartTime = _timer.elapsed_time();

    _displayDevice.displayTemperature(_currentTemperature);

    // simulate task computation by waiting for the required task computation time

    // std::chrono::microseconds elapsedTime =
    // std::chrono::microseconds::zero();
    // while (elapsedTime < kDisplayTask2ComputationTime) {
    //     elapsedTime = _timer.elapsed_time() - taskStartTime;
    // }
    std::chrono::milliseconds elapsedTime = std::chrono::milliseconds::zero();
    ThisThread::sleep_for(kDisplayTask2ComputationTime - elapsedTime);

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kDisplayTask2Index, taskStartTime);
}

}  // namespace static_scheduling
