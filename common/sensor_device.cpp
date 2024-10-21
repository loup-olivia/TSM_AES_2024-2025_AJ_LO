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
 * @file sensor_device.cpp
 * @author 
 *
 * @brief
 *
 * @date 
 * @version 
 ***************************************************************************/

#include "common/sensor_device.hpp"

namespace bike_computer {

    SensorDevice::SensorDevice() : _hdc1000(PD_13, PD_12, PC_6){}

    bool SensorDevice::init(){
        return _hdc1000.probe();
    }

    float SensorDevice::readHumidity(){
        return _hdc1000.getHumidity();
    }

    float SensorDevice::readTemperature(){
        return _hdc1000.getTemperature();

    }

}  // namespace bike_computer