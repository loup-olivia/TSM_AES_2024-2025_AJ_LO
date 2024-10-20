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
 * @file constants.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Constants definition used for implementing the bike system
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include <stdint.h>

#include "mbed.h"

namespace bike_computer {

// gear related constants
static constexpr uint8_t kMinGear = 1;
static constexpr uint8_t kMaxGear = 9;
// smallest gear (= 1) corresponds to a gear size of 20
// when the gear increases, the gear size descreases
static constexpr uint8_t kMaxGearSize = 20;
static constexpr uint8_t kMinGearSize = kMaxGearSize - kMaxGear;

// pedal related constants
// When compiling and linking with gcc, we get a link error when using static
// constexpr. The error is related to template instantiation.

// definition of pedal rotation initial time (corresponds to 80 turn / min)
static constexpr std::chrono::milliseconds kInitialPedalRotationTime = 750ms;
// definition of pedal minimal rotation time (corresponds to 160 turn / min)
static constexpr std::chrono::milliseconds kMinPedalRotationTime = 375ms;
// definition of pedal maximal rotation time (corresponds to 10 turn / min)
static constexpr std::chrono::milliseconds kMaxPedalRotationTime = 1500ms;
// definition of pedal rotation time change upon acceleration/deceleration
static constexpr std::chrono::milliseconds kDeltaPedalRotationTime = 25ms;

}  // namespace bike_computer