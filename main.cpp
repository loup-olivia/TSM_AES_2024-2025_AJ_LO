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
 * @brief main function for demonstrating WaitOnButton
 * @date 2022-09-01
 * @version 0.1.0
 ***************************************************************************/

#include "mbed.h"
#include "mbed_trace.h"
#include "wait_on_button.hpp"
#include "memory_logger.hpp"
#if MBED_CONF_MBED_TRACE_ENABLE
#undef TRACE_GROUP
#define TRACE_GROUP "main"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

int main() {
    // use trace library for console output
    mbed_trace_init();

    tr_debug("EventFlags program started\n");

    // log thread statistics
    advembsof::MemoryLogger memoryLogger;
    memoryLogger.getAndPrintThreadStatistics();

    // create the WaitOnButton instance and start it
    multi_tasking::WaitOnButton waitOnButton("ButtonThread");
    waitOnButton.start();

    // wait that the WaitOnButton thread started 
    waitOnButton.wait_started();

    // log thread statistics
    memoryLogger.getAndPrintThreadStatistics();

    // wait for the thread to exit (will not because of infinite loop)
    //waitOnButton.wait_exit();
    // or do busy waiting
    while (true) {
    }

    return 0;
}




/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
 
 
 
 //------------------------------------------------------------
 // BIKE SYSTEM MAIN PROGRAM
 //------------------------------------------------------------
 /*

#include <ctime>

#include "common/constants.hpp"
#include "mbed-os/mbed.h"
#include "mbed-trace/mbed_trace.h"
#include "static_scheduling/bike_system.hpp"
#include "static_scheduling_with_event/bike_system.hpp"

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
        static_scheduling_with_event::BikeSystem bikeSystem_with_event;

        bikeSystem_with_event.start();
        // bikeSystem.start();
        //  bikeSystem.startWithEventQueue();
    }
}
#endif*/