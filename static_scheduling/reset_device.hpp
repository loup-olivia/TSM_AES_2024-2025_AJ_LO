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
 * @file reset_device.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief ResetDevice header file (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "mbed.h"

namespace static_scheduling {

class ResetDevice {
   public:
    explicit ResetDevice(Timer& timer);  // NOLINT(runtime/references)

    // make the class non copyable
    ResetDevice(ResetDevice&)            = delete;
    ResetDevice& operator=(ResetDevice&) = delete;

    // method called for checking the reset status
    bool checkReset();

    // for computing the response time
    std::chrono::microseconds getPressTime();

   private:
    // called when the button is pressed
    void onRise();

    // data members
    // instance representing the reset button
    InterruptIn _resetButton;
    Timer& _timer;
    std::chrono::microseconds _pressTime;
};

}  // namespace static_scheduling
