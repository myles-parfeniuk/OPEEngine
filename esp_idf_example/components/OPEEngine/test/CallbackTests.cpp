// third-party
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
// OPEEngine
#include "../DataWatch.h"
#include "../OPEEngineConfig.h"
#include "OPEEngineTestHelper.hpp"

class CallbackTests
{
    public:
        static bool single_cb_same_task_test()
        {
            const constexpr char* TEST_TAG = "single_cb_same_task_test";

            bool cb_executed = false;
            SemaphoreHandle_t sem = xSemaphoreCreateBinary();

            TickType_t CB_EXECUTION_TIMEOUT_MS = 5UL / portTICK_PERIOD_MS;

            DataWatch<bool, 32, 1> data_to_set(false);
            CbHelper<OPEEconfigMAX_DATA_WATCH_CNT>::init();

            bool sub_success = data_to_set.subscribe<16>(
                    [&sem, &cb_executed](bool new_data)
                    {
                        cb_executed = true;
                        xSemaphoreGive(sem);
                    });

            if (!sub_success)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL:  subscribe() did not return successfully.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS:  successfully subscribed callback.");
            }

            if (!data_to_set.set(true))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL:  set() did not return successfully.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: successfully set data and queued callbacks.");
            }

            xSemaphoreTake(sem, CB_EXECUTION_TIMEOUT_MS);

            if (cb_executed != true)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: callback never executed");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: cb execution check.");
            }

            if (data_to_set.get() != true)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: no new data after cb execution.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: new data after cb execution check.");
            }

            vSemaphoreDelete(sem);
            
            return true;
        }

        static bool multiple_cb_same_task()
        {

            return true;
        }
};

TEST_CASE("single_cb_same_task_test", "[CallbackTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, CallbackTests::single_cb_same_task_test(), "FAILED single_cb_same_task_test");
}
