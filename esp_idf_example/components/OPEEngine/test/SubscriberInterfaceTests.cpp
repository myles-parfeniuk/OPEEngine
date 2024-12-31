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

namespace opee
{
    class SubscriberInterfaceTests
    {
        public:
            static bool notify_test()
            {
                const constexpr char* TEST_TAG = "notify_test";

                return true;
            }
    };

} // namespace opee

TEST_CASE("notify_test", "[SubscriberInterfaceTests]")
{
    TEST_ASSERT_EQUAL_MESSAGE(true, opee::SubscriberInterfaceTests::notify_test(), "FAILED notify_test");
}