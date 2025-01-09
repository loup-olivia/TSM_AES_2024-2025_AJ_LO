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
 * @brief Header file for defining the class representing application
 *        candidates.
 *
 * @date 2022-07-05
 * @version 0.1.0
 ***************************************************************************/

#pragma once

#include "block_device_application.hpp"
#include "mbed.h"
#include "candidate_applications.hpp"
#include "BlockDevice.h"

namespace update_client {

class MyCandidateApplications : public CandidateApplications {
   public:

    // storage address is specified relatively to the start of the block device
    MyCandidateApplications(BlockDevice& blockDevice,
                          mbed::bd_addr_t storageAddress,
                          mbed::bd_size_t storageSize,
                          uint32_t headerSize,
                          uint32_t nbrOfSlots);

    //~MyCandidateApplications() override;
    
    #if (USE_USB_SERIAL_UC == 1) 
        uint32_t getSlotForCandidate() override;
    #endif

   private : 
        mbed::bd_addr_t _storageAddress;
        mbed::bd_size_t _storageSize;
        uint32_t _headerSize;
        uint32_t _nbrOfSlots;

};

//Funciton exists with method WEAK thus by 
//creating a new function it overrides the WEAK one
//We only want to point to MyCandidateApplication. 
//Nothing else to change
MyCandidateApplications* createCandidateApplications(
    BlockDevice& blockDevice,
    mbed::bd_addr_t storageAddress,
    mbed::bd_size_t storageSize,
    uint32_t headerSize,
    uint32_t nbrOfSlots);

}  // namespace update_client

