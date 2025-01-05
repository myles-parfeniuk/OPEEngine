// in house
#include "app_main.h"
#include "Device.h"
#include "SerialService.h"
// third party
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "gpio.h"
#include "usart.h"
#include "OPEEngine.h"

static const constexpr char *TAG = "Main";

void test_task(void *arg);

int app_main()
{
    SerialService::init(&huart2);
    opee::OPEEngine_init();

    static Device d;

    d.usr_button.subscribe<16>([](ButtonEvent new_event)
                               { 
                                const constexpr char*CB_TAG = "CB_0";  
                                SerialService::print_log_ln(TAG, "%s: %s", CB_TAG, ButtonEvent_to_str(new_event)); });

    d.usr_button.subscribe<16>([](ButtonEvent new_event)
                               {   const constexpr char*CB_TAG = "CB_1";  
                                SerialService::print_log_ln(TAG, "%s: %s", CB_TAG, ButtonEvent_to_str(new_event)); });

    d.usr_button.subscribe<16>([](ButtonEvent new_event)
                               {   const constexpr char*CB_TAG = "CB_2";  
                                SerialService::print_log_ln(TAG, "%s: %s", CB_TAG, ButtonEvent_to_str(new_event)); });

    xTaskCreate(
        test_task,
        "TestTask",
        128,
        &d,
        1,
        NULL);

    vTaskStartScheduler();

    return 0;
}

void test_task(void *arg)
{
    Device *_d = static_cast<Device *>(arg);

    while (1)
    {
        _d->usr_button.set(ButtonEvent::quick_press);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        _d->usr_button.set(ButtonEvent::long_press);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        _d->usr_button.set(ButtonEvent::held);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        _d->usr_button.set(ButtonEvent::release);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}