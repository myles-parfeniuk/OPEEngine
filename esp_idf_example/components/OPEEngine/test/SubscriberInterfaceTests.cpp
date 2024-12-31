// third-party
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
// OPEEngine
#include "../DataWatch.h"
#include "../OPEEngineConfig.h"
#include "OPEEngineTestHelper.h"
#include "Subscriber.h"

namespace opee
{
    class SubscriberInterfaceTests
    {
        public:
            static bool validate_checksum_test()
            {
                const constexpr char* TEST_TAG = "validate_checksum_test";
                const constexpr TickType_t CB_EXECUTION_TIMEOUT_MS = 5UL / portTICK_PERIOD_MS;

                SemaphoreHandle_t sem = xSemaphoreCreateBinary();
                
                opee::Subscriber<bool> sub;
                opee::DataWatch<bool, 32, 1> data_to_set(false);
                opee::CbHelper<OPEEconfigMAX_DATA_WATCH_CNT>::init();

                if (data_to_set.get() != false)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: incorrect inital data in DataWatch object.");
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: DataWatch data initial value check.");
                }

                OPEEngineRes_t OPEEres = data_to_set.subscribe<16>([&sem](bool new_data) { xSemaphoreGive(sem); }, &sub);

                if (OPEEres != OPEE_OK)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL:  subscribe() did not return successfully: %s.", OPEEngineRes_to_str(OPEEres));
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS:  successfully subscribed callback.");
                }

                if (data_to_set.set(true) != OPEE_OK)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL:  set() did not return successfully.");
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: set data & queue cb check.");
                }

                if (xSemaphoreTake(sem, CB_EXECUTION_TIMEOUT_MS) != pdTRUE)
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

                if (sub.validate_checksum() != true)
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "FAIL: validate_checksum() returned false.");
                    return false;
                }
                else
                {
                    OPEEngineTestHelper::print_test_msg(TEST_TAG, "PASS: checksum validation check.");
                }
                vSemaphoreDelete(sem);

                return true;
            }

            static bool notify_test()
            {
                const constexpr char* TEST_TAG = "notify_test";

                return true;
            }
    };

} // namespace opee

TEST_CASE("validate_checksum_test", "[SubscriberInterfaceTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, opee::SubscriberInterfaceTests::validate_checksum_test(), "FAILED notify_test");
}

TEST_CASE("notify_test", "[SubscriberInterfaceTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, opee::SubscriberInterfaceTests::notify_test(), "FAILED notify_test");
}