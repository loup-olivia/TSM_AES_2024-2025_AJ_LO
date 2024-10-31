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
 * @brief Simple example of test program for raw and shared pointers
 *
 * @date 2022-09-01
 * @version 0.1.0
 ***************************************************************************/

#include "greentea-client/test_env.h"
#include "mbed.h"
#include "unity/unity.h"
#include "utest/utest.h"

using namespace utest::v1;
struct Test {
    Test() {
        _instanceCount++;
        _value = kMagicNumber;
    }

    ~Test() {
        _instanceCount--;
        _value = 0;
    }

    int _value;
    static constexpr uint32_t kMagicNumber = 33;
    static uint32_t _instanceCount;
};
uint32_t Test::_instanceCount = 0;

/**
 * Test that a shared pointer correctly manages the lifetime of the underlying raw pointer
 */
void test_single_sharedptr_lifetime() {
    // Sanity-check value of counter
    TEST_ASSERT_EQUAL(0, Test::_instanceCount);

    // Create and destroy shared pointer in given scope
    {
        std::shared_ptr<Test> shared_ptr(new Test);
        TEST_ASSERT_EQUAL(1, Test::_instanceCount);
        TEST_ASSERT_EQUAL(Test::kMagicNumber, shared_ptr->_value);
    }

    // Destroy shared pointer
    TEST_ASSERT_EQUAL(0, Test::_instanceCount);
}

/**
 * Test that multiple instances of shared pointers correctly manage the reference count
 * to release the object at the correct point
 */
void test_instance_sharing() {
    std::shared_ptr<Test> shared_ptr1(nullptr);

    // Sanity-check value of counter
    TEST_ASSERT_EQUAL(0, Test::_instanceCount);

    // Create and destroy shared pointer in given scope
    {
        std::shared_ptr<Test> shared_ptr2(new Test);
        TEST_ASSERT_EQUAL(1, Test::_instanceCount);
        // share share_ptr2 with shared_ptr1
        shared_ptr1 = shared_ptr2;
        // still one instance only
        TEST_ASSERT_EQUAL(1, Test::_instanceCount);
        TEST_ASSERT_EQUAL(Test::kMagicNumber, shared_ptr1->_value);
        TEST_ASSERT(shared_ptr1.get() == shared_ptr2.get());
    }

    // shared_ptr1 still owns a raw pointer
    TEST_ASSERT_EQUAL(1, Test::_instanceCount);

    shared_ptr1 = nullptr;

    // Shared pointer has been destroyed
    TEST_ASSERT_EQUAL(0, Test::_instanceCount);
}
// unused function
void test_raw_pointers() {
    // Sanity-check value of counter
    TEST_ASSERT_EQUAL(0, Test::_instanceCount);

    // Create and destroy shared pointer in given scope
    {
        Test* raw_ptr = new Test;
        TEST_ASSERT_EQUAL(1, Test::_instanceCount);
        TEST_ASSERT_EQUAL(Test::kMagicNumber, raw_ptr->_value);
        delete raw_ptr;
    }

    // Destroy shared pointer
    TEST_ASSERT_EQUAL(0, Test::_instanceCount);
}

/**
 * Test that multiple instances of unique pointers correctly manage the
 * ownership of the the object
 */
void test_unique_ptr() {
    std::unique_ptr<Test> unique_ptr1(nullptr);

    // Sanity-check value of counter
    TEST_ASSERT_EQUAL(0, Test::_instanceCount);

    // Create and destroy shared pointer in given scope
    {
        std::unique_ptr<Test> unique_ptr2(new Test);
        TEST_ASSERT_EQUAL(1, Test::_instanceCount);
        // move ownership of unique_ptr2 to unique_ptr1
        // unique_ptr1 = unique_ptr2; is not allowed
        // must use the std::move() semantics
        unique_ptr1 = std::move(unique_ptr2);
        // still one instance only
        TEST_ASSERT_EQUAL(1, Test::_instanceCount);
        TEST_ASSERT_EQUAL(Test::kMagicNumber, unique_ptr1->_value);
        // unique_ptr2 does not own ptr any more
        TEST_ASSERT_EQUAL(nullptr, unique_ptr2.get());
    }

    // unique_ptr1 still owns an instance
    TEST_ASSERT_EQUAL(1, Test::_instanceCount);

    // release unique_ptr1
    unique_ptr1 = nullptr;

    // unique_ptr instance has been released
    TEST_ASSERT_EQUAL(0, Test::_instanceCount);
}

static utest::v1::status_t greentea_setup(const size_t number_of_cases) {
    // Here, we specify the timeout (60s) and the host test (a built-in host test or the
    // name of our Python file)
    GREENTEA_SETUP(60, "default_auto");
    return greentea_test_setup_handler(number_of_cases);
}

// List of test cases in this file
static Case cases[] = {
    Case("Test single shared pointer instance", test_single_sharedptr_lifetime),
    Case("Test instance sharing across multiple shared pointers", test_instance_sharing)};

static Specification specification(greentea_setup, cases);

int main() { return !Harness::run(specification); }
