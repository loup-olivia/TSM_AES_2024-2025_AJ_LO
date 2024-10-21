/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "constants.hpp"
#include "bike_system.hpp"


// Blinking rate in milliseconds
#define BLINKING_RATE     500ms

#if !MBED_TEST_MODE

int main()
{
    // Initialise the digital pin LED1 as an output
#ifdef LED1
    DigitalOut led(LED1);
#else
    bool led;
#endif

    while (true) {
        static_scheduling::BikeSystem bikeSystem;
        bikeSystem.start();
    }
}
#endif