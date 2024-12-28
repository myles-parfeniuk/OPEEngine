#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "OPEEDevice.h"
#include "LED.h"
#include "Button.h"
#include "SerialService.h"
#include "test/OPEEngineTestSuite.hpp"

static const constexpr char* TAG = "Main";

extern "C" void app_main(void)
{
    //OPEEngineTestSuite::run_all_tests();

    static Device d;
    static LED led(d, GPIO_NUM_25);
    static Button button(d, GPIO_NUM_18);
    static SerialService serial_service(d);

    while (1)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
