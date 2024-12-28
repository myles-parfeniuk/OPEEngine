/**
 * @file OPEEngineTestHelper.hpp
 * @author Myles Parfeniuk
 */

#pragma once

#include <stdio.h>
#include <string.h>
#include "unity.h"

/**
 * @class OPEEngineTestHelper
 *
 * @brief OPEEngine unit test helper class.
 * */
class OPEEngineTestHelper
{
    private:
        static const constexpr char* TAG = "OPEEngineTestHelper";

    public:

         /**
         * @brief Prints a message during a test.
         *
         * @param TEST_TAG String containing test name.
         * @param msg String containing message to print.
         *
         * @return void, nothing to return
         */
        static void print_test_msg(const char* TEST_TAG, const char* msg)
        {
            static char msg_buff[100];
            sprintf(msg_buff, "%s: %s: %s", TAG, TEST_TAG, msg);
            UnityPrint(msg_buff);
            UNITY_OUTPUT_CHAR('\n');
            UNITY_OUTPUT_CHAR('\r');
        }
};