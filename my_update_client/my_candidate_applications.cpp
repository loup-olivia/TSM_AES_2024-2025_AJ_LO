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
 * @file candidate_applications.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Implementation of the class representing application candidates
 *
 * @date 2022-07-05
 * @version 0.1.0
 ***************************************************************************/
#include "my_candidate_applications.hpp"

// for unique_ptr
#include <memory>

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "MyCandidateApplications"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

#include "uc_error_code.hpp"


namespace update_client {

    MyCandidateApplications* createCandidateApplication(
        BlockDevice& blockDevice,
        mbed::bd_addr_t storageAddress,
        mbed::bd_size_t storageSize,
        uint32_t headerSize,
        uint32_t nbrOfSlots) {
        return new MyCandidateApplications(
        blockDevice, storageAddress, storageSize, headerSize, nbrOfSlots);
        }

// Constructor definition

    //The candidate app is created with the factory above
    //parameters such as storageAddress, storageSize, headerSize & nbrOfSlots
    //are defined in mbed_app.json 
    MyCandidateApplications::MyCandidateApplications(BlockDevice& blockDevice,
                                               mbed::bd_addr_t storageAddress,
                                               mbed::bd_size_t storageSize,
                                               uint32_t headerSize,
                                               uint32_t nbrOfSlots)
    : CandidateApplications(blockDevice, storageAddress, storageSize, headerSize, nbrOfSlots) {
    // Additional initialization for MyCandidateApplication, if needed
    tr_info("Constr MyCandidate");
}

    /*MyCandidateApplications::~MyCandidateApplications() {
    // Destructor implementation (can be empty)
    }*/

    #if (USE_USB_SERIAL_UC == 1)

        uint32_t MyCandidateApplications::getSlotForCandidate(){
            tr_info("Getting slot for candidate");
        mbed::bd_size_t slot_size = _storageSize / _nbrOfSlots;
        for (uint32_t slotIndex = 0; slotIndex < _nbrOfSlots; slotIndex++) {
            mbed::bd_addr_t candidateAddress = _storageAddress + (slotIndex * slot_size);
            //if (isSlotFree(candidateAddress)) {
                tr_info("Looking for slots");
                return slotIndex;
           // }
        }
    // No free slot found
        tr_info("No free slots");
        return -1; // Or handle as appropriate
    }

    #endif
    
}


  // namespace update_client
