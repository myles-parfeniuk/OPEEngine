/**
 * @file OPEEngineTestSuite.hpp
 * @author Myles Parfeniuk
 *
 *
 * @warning YOU MUST ADD THE FOLLOWING LINE TO YOUR MAIN PROJECTS CMakeLists.txt IN ORDER FOR THIS TEST SUITE TO BE BUILT WITH PROJECT:
 *          set(TEST_COMPONENTS "OPEEngine" CACHE STRING "Components to test.")
 */
#pragma once

// std lib
#include <stdio.h>
// third-party
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
            run_cb_pool_manager_tests(false);
            run_cb_tests(false);
            run_sub_interface_tests(false);
            UNITY_END();
        }

        static void run_cb_pool_manager_tests(bool call_unity_end_begin = true)
        {
            static const constexpr char* TESTS_TAG = "cb_pool_manager_tests";

            print_begin_tests_banner(TESTS_TAG);

            if (call_unity_end_begin)
                UNITY_BEGIN();

            unity_run_tests_by_tag("[PoolManagerTests]", false);

            if (call_unity_end_begin)
                UNITY_END();

            print_end_tests_banner(TESTS_TAG);
        }

        static void run_cb_tests(bool call_unity_end_begin = true)
        {
            static const constexpr char* TESTS_TAG = "cb_tests";

            print_begin_tests_banner(TESTS_TAG);

            if (call_unity_end_begin)
                UNITY_BEGIN();

            unity_run_tests_by_tag("[CallbackTests]", false);

            if (call_unity_end_begin)
                UNITY_END();

            print_end_tests_banner(TESTS_TAG);
        }

        static void run_sub_interface_tests(bool call_unity_end_begin = true)
        {
            static const constexpr char* TESTS_TAG = "sub_interface_tests";

            print_begin_tests_banner(TESTS_TAG);

            if (call_unity_end_begin)
                UNITY_BEGIN();

            unity_run_tests_by_tag("[SubscriberInterfaceTests]", false);

            if (call_unity_end_begin)
                UNITY_END();

            print_end_tests_banner(TESTS_TAG);
        }
};