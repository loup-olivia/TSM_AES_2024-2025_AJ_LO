/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bike_system.hpp"
#include "constants.hpp"
#include "mbed.h"
#include "mbed_trace.h"

// Blinking rate in milliseconds
#define BLINKING_RATE 500ms

#if defined(MBED_CONF_MBED_TRACE_ENABLE)
#define TRACE_GROUP "MAIN"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

#if !MBED_TEST_MODE
int main() {
    // Initialise the digital pin LED1 as an output
    // #ifdef LED1
    //     DigitalOut led(LED1);
    // #else
    //     bool led = false;
    // #endif

#if defined(MBED_CONF_MBED_TRACE_ENABLE)
    mbed_trace_init();
#endif

    while (true) {
        static_scheduling::BikeSystem bikeSystem;
        bikeSystem.start();
    }
}
#endif
