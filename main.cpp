/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ctime>

#include "common/constants.hpp"
#include "mbed-os/mbed.h"
#include "mbed-trace/mbed_trace.h"
#include "static_scheduling/bike_system.hpp"
#include "static_scheduling_with_event/bike_system.hpp"
#include "multi_tasking/bike_system.hpp"

//  Blinking rate in milliseconds
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
        // static_scheduling::BikeSystem bikeSystem;
        //static_scheduling_with_event::BikeSystem bikeSystem_with_event;
        multi_tasking::BikeSystem bike_system_multi_tasking;
      
        bike_system_multi_tasking.start();
        //bikeSystem_with_event.start();
        // bikeSystem.start();
        //  bikeSystem.startWithEventQueue();
    }
}
#endif
