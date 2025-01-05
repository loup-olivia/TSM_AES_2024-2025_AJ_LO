/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ctime>

#include "FlashIAPBlockDevice.h"
#include "common/constants.hpp"
#include "mbed-os/mbed.h"
#include "mbed-trace/mbed_trace.h"
#include "memory_logger.hpp"
#include "multi_tasking/bike_system.hpp"
#include "static_scheduling/bike_system.hpp"
#include "static_scheduling_with_event/bike_system.hpp"
#include "update-client/usb_serial_uc.hpp"

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

    // log thread statistics
    advembsof::MemoryLogger memoryLogger;
    memoryLogger.getAndPrintThreadStatistics();
    memoryLogger.printDiffs();

#if defined(MBED_CONF_MBED_TRACE_ENABLE)
    mbed_trace_init();
#endif
#if (USE_USB_SERIAL_UC == 1) && defined(HEADER_ADDR)
    FlashIAPBlockDevice flashIAPBlockDevice(MBED_ROM_START, MBED_ROM_SIZE);
    update_client::USBSerialUC usbSerialUpdateClient(flashIAPBlockDevice);
    update_client::UCErrorCode rc = usbSerialUpdateClient.start();

    if (rc != update_client::UCErrorCode::UC_ERR_NONE) {
        tr_error("Cannot initialize update client: %d", rc);
    } else {
        tr_info("Update client started");
    }
#endif

    while (true) {
        // static_scheduling::BikeSystem bikeSystem;
        // static_scheduling_with_event::BikeSystem bikeSystem_with_event;
        multi_tasking::BikeSystem bike_system_multi_tasking;

        bike_system_multi_tasking.start();
        // bikeSystem_with_event.start();
        // bikeSystem.start();
        // bikeSystem.startWithEventQueue();
    }
}
#endif
