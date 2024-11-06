/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ctime>

#include "advdembsof_library/utils/cpu_logger.hpp"
#include "common/constants.hpp"
#include "mbed-os/mbed.h"
#include "mbed-trace/mbed_trace.h"
#include "static_scheduling/bike_system.hpp"
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

    Timer timer;  // Créer une instance de Timer

#if defined(MBED_CONF_MBED_TRACE_ENABLE)
    mbed_trace_init();
#endif
    while (true) {
        static_scheduling::BikeSystem bikeSystem;
        bikeSystem.start();
        // advembsof::CPULogger _cpuLogger(timer);
        // _cpuLogger.printStats();
    }
}
#endif
