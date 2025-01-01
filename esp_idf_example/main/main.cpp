#include <stdio.h>

// third-party
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// esp-idf
#include "esp_log.h"
#include "driver/gpio.h"
// OPEEngine
#include "test/OPEEngineTestSuite.h"
// example specific
#include "Device.h"
#include "LED.h"
#include "Button.h"
#include "SerialService.h"

static const constexpr char* TAG = "Main";

extern "C" void app_main(void)
{
    OPEEngineTestSuite::run_all_tests();

    static Device d;
    static LED led(d, GPIO_NUM_25);
    static Button button(d, GPIO_NUM_18);
    static SerialService serial_service(d);

    while (1)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
