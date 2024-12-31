/**
 * @file OPEEngineTestHelper.hpp
 * @author Myles Parfeniuk
 */

#pragma once

// std lib
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
// third-party
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/**
 * @class OPEEngineTestHelper
 *
 * @brief OPEEngine unit test helper class.
 * */
class OPEEngineTestHelper
{
    private:
        static const constexpr char* TAG = "OPEEngineTestHelper";
        inline static SemaphoreHandle_t print_lock = xSemaphoreCreateMutex();

    public:
        /**
         * @brief Prints a message during a test.
         *
         * @param TEST_TAG String containing test name.
         * @param msg String containing message to print.
         *
         * @return void, nothing to return
         */
        static void print_test_msg(const char* TEST_TAG, const char* format, ...)
        {
            static char msg_buff[200];

            xSemaphoreTake(print_lock, portMAX_DELAY);
            va_list args;
            va_start(args, format);

            snprintf(msg_buff, sizeof(msg_buff), "%s: %s: ", TAG, TEST_TAG);
            vsnprintf(msg_buff + strlen(msg_buff), sizeof(msg_buff) - strlen(msg_buff), format, args);
            va_end(args);

            UnityPrint(msg_buff);
            UNITY_OUTPUT_CHAR('\n');
            UNITY_OUTPUT_CHAR('\r');
            xSemaphoreGive(print_lock);
        }
};