/**
 * @file OPEEngineTestSuite.hpp
 * @author Myles Parfeniuk
 *
 *
 * @warning YOU MUST ADD THE FOLLOWING LINE TO YOUR MAIN PROJECTS CMakeLists.txt IN ORDER FOR THIS TEST SUITE TO BE BUILT WITH PROJECT:
 *          set(TEST_COMPONENTS "OPEEngine" CACHE STRING "Components to test.")
 */
#pragma once

#include <stdio.h>
#include <string.h>
#include "unity.h"

/**
 * @class OPEEngineTestSuite
 *
 * @brief OPEEngineTestSuite unit test launch point class.
 * */
class OPEEngineTestSuite
{
    private:
        static void print_begin_tests_banner(const char* test_set_name)
        {
            printf("####################### BEGIN TESTS: %s #######################\n\r", test_set_name);
        }

        static void print_end_tests_banner(const char* test_set_name)
        {
            printf("####################### END TESTS: %s #######################\n\r", test_set_name);
        }

    public:
        static void run_all_tests()
        {
            UNITY_BEGIN();
            run_allocator_tests(false);
            UNITY_END();
        }

        static void run_allocator_tests(bool call_unity_end_begin = true)
        {
            static const constexpr char* TESTS_TAG = "allocator_tests";

            print_begin_tests_banner(TESTS_TAG);

            if (call_unity_end_begin)
                UNITY_BEGIN();

            unity_run_tests_by_tag("[DWStkAllocatorTests]", false);

            if (call_unity_end_begin)
                UNITY_END();

            print_end_tests_banner(TESTS_TAG);
        }
};