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
 * @file pedal_device.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Pedal System header file (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "constants.hpp"
#include "mbed.h"

namespace static_scheduling_with_event {

class PedalDevice {
   public:
    explicit PedalDevice(Timer& timer);  // NOLINT(runtime/references)

    // make the class non copyable
    PedalDevice(PedalDevice&)            = delete;
    PedalDevice& operator=(PedalDevice&) = delete;

    // method called for updating the bike system
    std::chrono::milliseconds getCurrentRotationTime();

   private:
    // private methods
    void increaseRotationSpeed();
    void decreaseRotationSpeed();

    // data members
    std::chrono::milliseconds _pedalRotationTime =
        bike_computer::kInitialPedalRotationTime;
    Timer& _timer;
};

}  // namespace static_scheduling_with_event
