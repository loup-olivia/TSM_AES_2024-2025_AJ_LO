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
 * @file main.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Bike computer test suite: speedometer device
 *
 * @date 2023-08-26
 * @version 0.1.0
 ***************************************************************************/

#include <chrono>

#include "common/constants.hpp"
#include "common/speedometer.hpp"
#include "greentea-client/test_env.h"
#include "mbed.h"
#include "static_scheduling/gear_device.hpp"
#include "unity/unity.h"
#include "utest/utest.h"

using namespace utest::v1;

// allow for 0.1 km/h difference
static constexpr float kAllowedSpeedDelta = 0.1f;
// allow for 1m difference
static constexpr float kAllowedDistanceDelta = 1.0f / 1000.0;

// function called by test handler functions for verifying the current speed
void check_current_speed(const std::chrono::milliseconds& pedalRotationTime,
                         uint8_t traySize,
                         uint8_t gearSize,
                         float wheelCircumference,
                         float currentSpeed) {
    // compute the number of pedal rotation per hour
    uint32_t milliSecondsPerHour = 1000 * 3600;
    float pedalRotationsPerHour  = static_cast<float>(milliSecondsPerHour) /
                                  static_cast<float>(pedalRotationTime.count());

    // compute the expected speed in km / h
    // first compute the distance in meter for each pedal turn
    float trayGearRatio = static_cast<float>(traySize) / static_cast<float>(gearSize);
    float distancePerPedalTurn = trayGearRatio * wheelCircumference;
    float expectedSpeed        = (distancePerPedalTurn / 1000.0f) * pedalRotationsPerHour;

    printf("  Expected speed is %f, current speed is %f\n", expectedSpeed, currentSpeed);
    TEST_ASSERT_FLOAT_WITHIN(kAllowedSpeedDelta, expectedSpeed, currentSpeed);
}

// compute the traveled distance for a time interval
float compute_distance(const std::chrono::milliseconds& pedalRotationTime,
                       uint8_t traySize,
                       uint8_t gearSize,
                       float wheelCircumference,
                       const std::chrono::milliseconds& travelTime) {
    // compute the number of pedal rotation during travel time
    // both times are expressed in ms
    float pedalRotations = static_cast<float>(travelTime.count()) /
                           static_cast<float>(pedalRotationTime.count());

    // compute the distance in meter for each pedal turn
    float trayGearRatio = static_cast<float>(traySize) / static_cast<float>(gearSize);
    float distancePerPedalTurn = trayGearRatio * wheelCircumference;

    // distancePerPedalTurn is expressed in m, divide per 1000 for a distance in km
    return (distancePerPedalTurn * pedalRotations) / 1000.0;
}

// function called by test handler functions for verifying the distance traveled
void check_distance(const std::chrono::milliseconds& pedalRotationTime,
                    uint8_t traySize,
                    uint8_t gearSize,
                    float wheelCircumference,
                    const std::chrono::milliseconds& travelTime,
                    float distance) {
    // distancePerPedalTurn is expressed in m, divide per 1000 for a distance in km
    float expectedDistance = compute_distance(
        pedalRotationTime, traySize, gearSize, wheelCircumference, travelTime);
    printf("  Expected distance is %f, current distance is %f\n",
           expectedDistance,
           distance);
    TEST_ASSERT_FLOAT_WITHIN(kAllowedDistanceDelta, expectedDistance, distance);
}

// test the speedometer by modifying the gear
static control_t test_gear_size(const size_t call_count) {
    // create a timer
    Timer timer;
    // start the timer
    timer.start();

    // create a speedometer instance
    bike_computer::Speedometer speedometer(timer);

    // get speedometer constant values (for this test)
    const auto traySize           = speedometer.getTraySize();
    const auto wheelCircumference = speedometer.getWheelCircumference();
    const auto pedalRotationTime  = speedometer.getCurrentPedalRotationTime();

    for (uint8_t gearSize = bike_computer::kMinGearSize;
         gearSize <= bike_computer::kMaxGearSize;
         gearSize++) {
        // set the gear
        printf("Testing gear size %d\n", gearSize);
        speedometer.setGearSize(gearSize);

        // get the current speed
        auto currentSpeed = speedometer.getCurrentSpeed();

        // check the speed against the expected one
        check_current_speed(
            pedalRotationTime, traySize, gearSize, wheelCircumference, currentSpeed);
    }

    // execute the test only once and move to the next one, without waiting
    return CaseNext;
}

// test the speedometer by modifying the pedal rotation speed
static control_t test_rotation_speed(const size_t call_count) {
    // create a timer
    Timer timer;
    // start the timer
    timer.start();

    // create a speedometer instance
    bike_computer::Speedometer speedometer(timer);

    // set the gear size
    speedometer.setGearSize(bike_computer::kMaxGearSize);

    // get speedometer constant values
    const auto traySize           = speedometer.getTraySize();
    const auto wheelCircumference = speedometer.getWheelCircumference();
    const auto gearSize           = speedometer.getGearSize();

    // first test increasing rotation speed (decreasing rotation time)
    auto pedalRotationTime = speedometer.getCurrentPedalRotationTime();
    while (pedalRotationTime > bike_computer::kMinPedalRotationTime) {
        // decrease the pedal rotation time
        pedalRotationTime -= bike_computer::kDeltaPedalRotationTime;
        speedometer.setCurrentRotationTime(pedalRotationTime);

        // get the current speed
        const auto currentSpeed = speedometer.getCurrentSpeed();

        // check the speed against the expected one
        check_current_speed(
            pedalRotationTime, traySize, gearSize, wheelCircumference, currentSpeed);
    }

    // second test decreasing rotation speed (increasing rotation time)
    pedalRotationTime = speedometer.getCurrentPedalRotationTime();
    while (pedalRotationTime < bike_computer::kMaxPedalRotationTime) {
        // increase the pedal rotation time
        pedalRotationTime += bike_computer::kDeltaPedalRotationTime;
        speedometer.setCurrentRotationTime(pedalRotationTime);

        // get the current speed
        const auto currentSpeed = speedometer.getCurrentSpeed();

        // check the speed against the expected one
        check_current_speed(
            pedalRotationTime, traySize, gearSize, wheelCircumference, currentSpeed);
    }

    // execute the test only once and move to the next one, without waiting
    return CaseNext;
}

// test the speedometer by modifying the pedal rotation speed
static control_t test_distance(const size_t call_count) {
    // create a timer
    Timer timer;

    // create a speedometer instance
    bike_computer::Speedometer speedometer(timer);

    // set the gear size
    speedometer.setGearSize(bike_computer::kMaxGearSize);

    // get speedometer constant values
    const auto traySize           = speedometer.getTraySize();
    const auto wheelCircumference = speedometer.getWheelCircumference();
    auto gearSize                 = speedometer.getGearSize();
    auto pedalRotationTime        = speedometer.getCurrentPedalRotationTime();

    // test different travel times
    const std::chrono::milliseconds travelTimes[] = {500ms, 1000ms, 5s, 10s};
    const uint8_t nbrOfTravelTimes = sizeof(travelTimes) / sizeof(travelTimes[0]);

    // start the timer (for simulating bike start)
    timer.start();

    // first check travel distance without changing gear and rotation speed
    std::chrono::milliseconds totalTravelTime = std::chrono::milliseconds::zero();
    for (uint8_t index = 0; index < nbrOfTravelTimes; index++) {
        // run for the travel time and get the distance
        ThisThread::sleep_for(travelTimes[index]);

        // get the distance traveled
        const auto distance = speedometer.getDistance();

        // accumulate travel time
        totalTravelTime += travelTimes[index];

        // check the distance vs the expected one
        check_distance(pedalRotationTime,
                       traySize,
                       gearSize,
                       wheelCircumference,
                       totalTravelTime,
                       distance);
    }

    // now change gear at each time interval
    auto expectedDistance = speedometer.getDistance();
    for (uint8_t index = 0; index < nbrOfTravelTimes; index++) {
        // update the gear size
        gearSize++;
        speedometer.setGearSize(gearSize);

        // run for the travel time and get the distance
        ThisThread::sleep_for(travelTimes[index]);

        // compute the expected distance for this time segment
        float distance = compute_distance(pedalRotationTime,
                                          traySize,
                                          gearSize,
                                          wheelCircumference,
                                          travelTimes[index]);
        expectedDistance += distance;

        // get the distance traveled
        const auto traveledDistance = speedometer.getDistance();

        printf("  Expected distance is %f, current distance is %f\n",
               expectedDistance,
               traveledDistance);
        TEST_ASSERT_FLOAT_WITHIN(
            kAllowedDistanceDelta, expectedDistance, traveledDistance);
    }
    // now change rotation speed at each time interval
    expectedDistance = speedometer.getDistance();
    for (uint8_t index = 0; index < nbrOfTravelTimes; index++) {
        // update the rotation speed
        pedalRotationTime += bike_computer::kDeltaPedalRotationTime;
        speedometer.setCurrentRotationTime(pedalRotationTime);

        // run for the travel time and get the distance
        ThisThread::sleep_for(travelTimes[index]);

        // compute the expected distance for this time segment
        float distance = compute_distance(pedalRotationTime,
                                          traySize,
                                          gearSize,
                                          wheelCircumference,
                                          travelTimes[index]);
        expectedDistance += distance;

        // get the distance traveled
        const auto traveledDistance = speedometer.getDistance();

        printf("  Expected distance is %f, current distance is %f\n",
               expectedDistance,
               traveledDistance);
        TEST_ASSERT_FLOAT_WITHIN(
            kAllowedDistanceDelta, expectedDistance, traveledDistance);
    }

    // execute the test only once and move to the next one, without waiting
    return CaseNext;
}

// test the speedometer by modifying the pedal rotation speed
static control_t test_reset(const size_t call_count) {
    // create a timer instance
    Timer timer;

    // create a speedometer instance
    bike_computer::Speedometer speedometer(timer);

    // set the gear size
    speedometer.setGearSize(bike_computer::kMinGearSize);

    // get speedometer constant values
    const auto traySize           = speedometer.getTraySize();
    const auto wheelCircumference = speedometer.getWheelCircumference();
    const auto gearSize           = speedometer.getGearSize();
    const auto pedalRotationTime  = speedometer.getCurrentPedalRotationTime();

    // start the timer (for simulating bike start)
    timer.start();

    // travel for 1 second
    const auto travelTime = 1000ms;
    ThisThread::sleep_for(travelTime);

    // check the expected distaance traveled
    const auto expectedDistance = compute_distance(
        pedalRotationTime, traySize, gearSize, wheelCircumference, travelTime);

    // get the distance traveled
    auto traveledDistance = speedometer.getDistance();

    printf("  Expected distance is %f, current distance is %f\n",
           expectedDistance,
           traveledDistance);
    TEST_ASSERT_FLOAT_WITHIN(kAllowedDistanceDelta, expectedDistance, traveledDistance);

    // reset the speedometer
    speedometer.reset();

    // traveled distance should now be zero
    traveledDistance = speedometer.getDistance();

    printf("  Expected distance is %f, current distance is %f\n", 0.0f, traveledDistance);
    TEST_ASSERT_FLOAT_WITHIN(kAllowedDistanceDelta, 0.0f, traveledDistance);

    // execute the test only once and move to the next one, without waiting
    return CaseNext;
}

static utest::v1::status_t greentea_setup(const size_t number_of_cases) {
    // Here, we specify the timeout (60s) and the host test (a built-in host test or the
    // name of our Python file)
    GREENTEA_SETUP(180, "default_auto");

    return greentea_test_setup_handler(number_of_cases);
}

// List of test cases in this file
static Case cases[] = {
    Case("test speedometer gear size change", test_gear_size),
    Case("test speedometer rotation speed change", test_rotation_speed),
    Case("test speedometer distance", test_distance),
    Case("test speedometer reset", test_reset)};

static Specification specification(greentea_setup, cases);

int main() { return !Harness::run(specification); }