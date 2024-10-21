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
 * @file sensor_device.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief SensorDevice header file (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "hdc1000.hpp"
#include "mbed.h"

namespace bike_computer {

class SensorDevice {
   public:
    // constructor
    SensorDevice();

    // method for initializing the device
    bool init();

    // methods used for
    float readTemperature();
    float readHumidity();

   private:
    // data members
    advembsof::HDC1000 _hdc1000;
};

}  // namespace bike_computer
