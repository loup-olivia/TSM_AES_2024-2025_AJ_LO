#include "FlashIAPBlockDevice.h"
#include "mbed.h"
#include "BlockDevice.h"
#include "mbed_trace.h"
#include "update-client/block_device_application.hpp"
#include "update-client/uc_error_code.hpp"
#include "update-client/usb_serial_uc.hpp"
#include "update-client/my_candidate_applications.hpp"


#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "bootloader"
#endif // MBED_CONF_MBED_TRACE_ENABLE


#if MBED_CONF_MBED_TRACE_ENABLE
static UnbufferedSerial g_uart(CONSOLE_TX, CONSOLE_RX);

// Function that directly outputs to an unbuffered serial port in blocking mode.
static void boot_debug(const char *s)
{
    size_t len = strlen(s);
    g_uart.write(s, len);
    g_uart.write("\r\n", 2);
}
#endif

int main()
{
#if MBED_CONF_MBED_TRACE_ENABLE
    mbed_trace_init();
    mbed_trace_print_function_set(boot_debug);
#endif // MBED_CONF_MBED_TRACE_ENABLE
    tr_debug("BikeComputer bootloader\r\n");

    FlashIAPBlockDevice flashIAPBlockDevice(MBED_ROM_START, MBED_ROM_SIZE);

    if(flashIAPBlockDevice.init() == 0){
        mbed::bd_addr_t headerAddress = HEADER_ADDR - MBED_ROM_START; //Starting relative to ROM_START
        mbed::bd_addr_t applicationAddress = POST_APPLICATION_ADDR - MBED_ROM_START;
        update_client::BlockDeviceApplication activeApplciation(
            flashIAPBlockDevice, headerAddress, applicationAddress);
        
        update_client::UCErrorCode rc = activeApplciation.checkApplication();

        tr_info("Active application UCErrorCode : %" PRIu8 "\n", rc);

        if(update_client::UCErrorCode::UC_ERR_NONE != rc){
            tr_error("Active application is not valid");
        }
        else {
            tr_debug("Active application is valid");
        }


        //Used for installing on reboot
        const mbed::bd_addr_t storage_address = MBED_CONF_UPDATE_CLIENT_STORAGE_ADDRESS;
        const mbed::bd_size_t storage_size = MBED_CONF_UPDATE_CLIENT_STORAGE_SIZE;
        const uint32_t header_size = POST_APPLICATION_ADDR - HEADER_ADDR;
        const uint32_t nbr_of_slots = MBED_CONF_UPDATE_CLIENT_STORAGE_LOCATIONS;

        update_client::CandidateApplications candidateApplications(flashIAPBlockDevice, storage_address, storage_size, header_size, nbr_of_slots); 

        uint32_t new_slot_index = 0;
        if(candidateApplications.hasValidNewerApplication(activeApplciation, new_slot_index)){
            tr_debug("New application available in slot %d", new_slot_index);
            activeApplciation.compareTo(candidateApplications.getBlockDeviceApplication(new_slot_index));

            rc = candidateApplications.installApplication(new_slot_index, headerAddress);
            if (rc == update_client::UCErrorCode::UC_ERR_NONE)
            {
                tr_debug("New application installed from slot %d", new_slot_index);
            }
            else
            {
                tr_error("Cannot install application from slot %d", new_slot_index);
            }
        } else {
            tr_debug("No new application available");
        }
    }

    // at this stage we directly branch to the main application
    void *sp = *((void **) POST_APPLICATION_ADDR + 0);  // NOLINT(readability/casting)
    void *pc = *((void **) POST_APPLICATION_ADDR + 1);  // NOLINT(readability/casting)
    tr_debug("Starting application at address 0x%08x (sp 0x%08x, pc 0x%08x)\r\n", POST_APPLICATION_ADDR, (uint32_t) sp, (uint32_t) pc);

    mbed_start_application(POST_APPLICATION_ADDR);

    return 0;
}