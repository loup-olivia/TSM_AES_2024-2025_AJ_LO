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
 * @file main.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Simple example of test program that always succeeds
 *
 * @date 2022-09-01
 * @version 0.1.0
 ***************************************************************************/

#include "greentea-client/test_env.h"
#include "mbed.h"
#include "unity/unity.h"
#include "utest/utest.h"

using namespace utest::v1;

// test handler function
static control_t always_succeed(const size_t call_count) {
    // this is the always succeed test
    TEST_ASSERT_EQUAL(4, 2 * 2);

    // execute the test only once and move to the next one, without waiting
    return CaseNext;
}

static utest::v1::status_t greentea_setup(const size_t number_of_cases) {
    // Here, we specify the timeout (60s) and the host test (a built-in host test or the
    // name of our Python file)
    GREENTEA_SETUP(60, "default_auto");

    return greentea_test_setup_handler(number_of_cases);
}

// List of test cases in this file
static Case cases[] = {Case("always succeed test", always_succeed)};

static Specification specification(greentea_setup, cases);

int main() { return !Harness::run(specification); }
