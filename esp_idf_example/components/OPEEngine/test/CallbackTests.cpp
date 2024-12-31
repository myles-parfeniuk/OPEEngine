// third-party
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
// OPEEngine
#include "../DataWatch.h"
#include "../OPEEngineConfig.h"
#include "OPEEngineTestHelper.hpp"

class CallbackTests
{
    public:
        static bool single_cb_test()
        {
            const constexpr char* TEST_TAG = "single_cb_test";

            bool cb_executed = false;
            SemaphoreHandle_t sem = xSemaphoreCreateBinary();

            TickType_t CB_EXECUTION_TIMEOUT_MS = 5UL / portTICK_PERIOD_MS;

            DataWatch<bool, 32, 1> data_to_set(false);
            CbHelper<OPEEconfigMAX_DATA_WATCH_CNT>::init();

            if (data_to_set.get() != false)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: incorrect inital data in DataWatch object.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DataWatch data initial value check.");
            }

            OPEEngineRes_t OPEEres = data_to_set.subscribe<16>(
                    [&sem, &cb_executed](bool new_data)
                    {
                        cb_executed = true;
                        xSemaphoreGive(sem);
                    });

            if (OPEEres != OPEE_SUCCESS)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL:  subscribe() did not return successfully: %s.", OPEEngineRes_to_str(OPEEres));
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
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: set data & queue cb check.");
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

        static bool multi_cb_test()
        {
            const constexpr char* TEST_TAG = "multi_cb_test";

            const constexpr EventBits_t CB_0_EXECUTED_BIT = 1U << 0U;
            const constexpr EventBits_t CB_1_EXECUTED_BIT = 1U << 1U;
            const constexpr EventBits_t CB_2_EXECUTED_BIT = 1U << 2U;
            const constexpr EventBits_t CB_ALL_EXECUTED_BITS = (CB_0_EXECUTED_BIT | CB_1_EXECUTED_BIT | CB_2_EXECUTED_BIT);
            const constexpr TickType_t CB_EXECUTION_TIMEOUT_MS = 5UL / portTICK_PERIOD_MS;

            OPEEngineRes_t OPEEres[3] = {OPEE_CB_WRPR_CREATION_FAILED};
            EventGroupHandle_t evt_grp_data_to_set = xEventGroupCreate();

            DataWatch<bool, 128, 3> data_to_set(false);
            CbHelper<OPEEconfigMAX_DATA_WATCH_CNT>::init();

            if (data_to_set.get() != false)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: incorrect inital data in DataWatch object.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DataWatch data initial value check.");
            }

            OPEEres[0] = data_to_set.subscribe<16>(
                    [&evt_grp_data_to_set](bool new_data)
                    {
                        xEventGroupSetBits(evt_grp_data_to_set, CB_0_EXECUTED_BIT);
                        OPEEngineTestHelper::print_test_msg(TEST_TAG, "CB_0 executed...");
                    });
            OPEEres[1] = data_to_set.subscribe<16>(
                    [&evt_grp_data_to_set](bool new_data)
                    {
                        xEventGroupSetBits(evt_grp_data_to_set, CB_1_EXECUTED_BIT);
                        OPEEngineTestHelper::print_test_msg(TEST_TAG, "CB_1 executed...");
                    });
            OPEEres[2] = data_to_set.subscribe<16>(
                    [&evt_grp_data_to_set](bool new_data)
                    {
                        xEventGroupSetBits(evt_grp_data_to_set, CB_2_EXECUTED_BIT);
                        OPEEngineTestHelper::print_test_msg(TEST_TAG, "CB_2 executed...");
                    });

            for (int i = 0; i < 3; i++)
            {
                if (OPEEres[i] != OPEE_SUCCESS)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL:  subscribe() did not return successfully for callback %d: %s.", i, OPEEngineRes_to_str(OPEEres[i]));
                    return false;
                }
            }

            OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS:  sub check.");

            if (!data_to_set.set(true))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL:  set() did not return successfully.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: set data & queue cb check.");
            }

            EventBits_t res = xEventGroupWaitBits(evt_grp_data_to_set, CB_ALL_EXECUTED_BITS, pdFALSE, pdTRUE, CB_EXECUTION_TIMEOUT_MS);

            if (res != CB_ALL_EXECUTED_BITS)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL:  not all callbacks executed.");
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

            vEventGroupDelete(evt_grp_data_to_set);

            return true;
        }

        static bool multi_task_cb_test()
        {
            const constexpr char* TEST_TAG = "multi_task_cb_test";

            const constexpr EventBits_t SUBSCRIBED_BIT = 1U << 0U;
            const constexpr EventBits_t DATA_SET_BIT = 1U << 1U;
            const constexpr EventBits_t CB_EXECUTED_BIT = 1U << 2U;
            const constexpr TickType_t DATA_SET_TIMEOUT_MS = 5UL / portTICK_PERIOD_MS;
            const constexpr TickType_t CB_EXECUTION_TIMEOUT_MS = 5UL / portTICK_PERIOD_MS;
            const constexpr TickType_t SUB_TIMEOUT_MS = 5UL / portTICK_PERIOD_MS;

            EventGroupHandle_t evt_grp_data_to_set = xEventGroupCreate();

            static DataWatch<bool, 128, 3> data_to_set(false);
            CbHelper<OPEEconfigMAX_DATA_WATCH_CNT>::init();

            multi_task_cb_test_task_ctx_t ctx = {&data_to_set, evt_grp_data_to_set};

            if (xTaskCreate(multi_task_cb_test_task_1, "multi_task_cb_test_task_0", 2048U, &ctx, 2, NULL) != pdTRUE)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: could not create test task 0.");
                return false;
            }

            if (xTaskCreate(multi_task_cb_test_task_0, "multi_task_cb_test_task_1", 2048U, &ctx, 2, NULL) != pdTRUE)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: could not create test task 1.");
                return false;
            }

            OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: test task creation check.");

            EventBits_t res = xEventGroupWaitBits(evt_grp_data_to_set, SUBSCRIBED_BIT, pdFALSE, pdFALSE, SUB_TIMEOUT_MS);

            if (!(res & SUBSCRIBED_BIT))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: task 1 did not successfully subscribe callback.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: sub check");
            }

            res = xEventGroupWaitBits(evt_grp_data_to_set, DATA_SET_BIT, pdFALSE, pdFALSE, DATA_SET_TIMEOUT_MS);

            if (!(res & DATA_SET_BIT))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: task 0 did not successfully set data.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: set data and queue cb check");
            }

            res = xEventGroupWaitBits(evt_grp_data_to_set, CB_EXECUTED_BIT, pdFALSE, pdFALSE, CB_EXECUTION_TIMEOUT_MS);

            if (!(res & CB_EXECUTED_BIT))
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: task 1 did not successfully detect cb execution.");
                return false;
            }
            else
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: cb execution check");
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

            vEventGroupDelete(evt_grp_data_to_set);
            return true;
        }

    private:
        typedef struct multi_task_cb_test_task_ctx_t
        {
                DataWatch<bool, 128, 3>* _data_to_set;
                EventGroupHandle_t evt_grp_data_to_set;
        } multi_task_cb_test_task_ctx_t;

        static void multi_task_cb_test_task_0(void* arg)
        {
            const constexpr char* TEST_TAG = "multi_task_cb_test";

            const constexpr EventBits_t SUBSCRIBED_BIT = 1U << 0U;
            const constexpr EventBits_t DATA_SET_BIT = 1U << 1U;
            const constexpr TickType_t SUB_TIMEOUT_MS = 5UL / portTICK_PERIOD_MS;
            const constexpr TickType_t CB_EXECUTION_TIMEOUT_MS = 5UL / portTICK_PERIOD_MS;

            multi_task_cb_test_task_ctx_t* _ctx = static_cast<multi_task_cb_test_task_ctx_t*>(arg);

            EventBits_t res = xEventGroupWaitBits(_ctx->evt_grp_data_to_set, SUBSCRIBED_BIT, pdFALSE, pdFALSE, SUB_TIMEOUT_MS);

            if (res & SUBSCRIBED_BIT)
            {
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "Sub detected...");

                if (_ctx->_data_to_set->set(true))
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "Set...");

                xEventGroupSetBits(_ctx->evt_grp_data_to_set, DATA_SET_BIT);
            }

            xEventGroupWaitBits(_ctx->evt_grp_data_to_set, SUBSCRIBED_BIT, pdFALSE, pdFALSE, CB_EXECUTION_TIMEOUT_MS);
            OPEEngineTestHelper::print_test_msg(TEST_TAG, "Test task 0 deleting...");
            vTaskDelete(NULL);
        }

        static void multi_task_cb_test_task_1(void* arg)
        {
            const constexpr char* TEST_TAG = "multi_task_cb_test";

            const constexpr EventBits_t SUBSCRIBED_BIT = 1U << 0U;
            const constexpr EventBits_t DATA_SET_BIT = 1U << 1U;
            const constexpr EventBits_t CB_EXECUTED_BIT = 1U << 2U;
            const constexpr TickType_t DATA_SET_TIMEOUT_MS = 5UL / portTICK_PERIOD_MS;
            const constexpr TickType_t CB_EXECUTION_TIMEOUT_MS = 5UL / portTICK_PERIOD_MS;

            multi_task_cb_test_task_ctx_t* _ctx = static_cast<multi_task_cb_test_task_ctx_t*>(arg);

            OPEEngineRes_t OPEEres = _ctx->_data_to_set->subscribe<16>(
                    [&_ctx](bool new_data)
                    {
                        OPEEngineTestHelper::print_test_msg(TEST_TAG, "CB executed...");
                        xEventGroupSetBits(_ctx->evt_grp_data_to_set, CB_EXECUTED_BIT);
                    });

            if (OPEEres == OPEE_SUCCESS)
            {
                xEventGroupSetBits(_ctx->evt_grp_data_to_set, SUBSCRIBED_BIT);
                OPEEngineTestHelper::print_test_msg(TEST_TAG, "Subbed...");

                EventBits_t res = xEventGroupWaitBits(_ctx->evt_grp_data_to_set, DATA_SET_BIT, pdFALSE, pdFALSE, DATA_SET_TIMEOUT_MS);

                if (res & DATA_SET_BIT)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "Set detected...");
                    xEventGroupWaitBits(_ctx->evt_grp_data_to_set, CB_EXECUTED_BIT, pdFALSE, pdFALSE, CB_EXECUTION_TIMEOUT_MS);
                }
            }

            OPEEngineTestHelper::print_test_msg(TEST_TAG, "Test task 1 deleting...");
            vTaskDelete(NULL);
        }
};

TEST_CASE("single_cb_test", "[CallbackTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, CallbackTests::single_cb_test(), "FAILED single_cb_test");
}

TEST_CASE("multi_cb_test", "[CallbackTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, CallbackTests::multi_cb_test(), "FAILED multi_cb_test");
}

TEST_CASE("multi_task_cb_test", "[CallbackTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, CallbackTests::multi_task_cb_test(), "FAILED multi_task_cb_test");
}