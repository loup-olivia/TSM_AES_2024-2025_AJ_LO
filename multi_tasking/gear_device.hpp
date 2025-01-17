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
 * @file gear_device.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Gear Device header file (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "InterruptIn.h"
#include "constants.hpp"
#include "mbed.h"

namespace multi_tasking {

class GearDevice {
   public:
    GearDevice(EventQueue& eventQueue, mbed::Callback<void(uint8_t, uint8_t)> cb);  // NOLINT(runtime/references)

    // make the class non copyable
    GearDevice(GearDevice&)            = delete;
    GearDevice& operator=(GearDevice&) = delete;

    // method called for updating the bike system
    uint8_t getCurrentGear();
    uint8_t getCurrentGearSize() const;

   private:
    void postEvent();

#if defined(MBED_TEST_MODE)
    public:
    void onJoystickUp();
    void onJoystickDown();
#else 
    private : 
    void onJoystickUp();
    void onJoystickDown();
#endif  // defined(MBED_TEST_MODE)
    

    // data members
    uint8_t _currentGear = bike_computer::kMinGear;

    // Eventqueue
    EventQueue& _eventQueue;
    // Callbacks
    mbed::Callback<void(uint8_t, uint8_t)> _cb;

};

}  // namespace static_scheduling_with_event
