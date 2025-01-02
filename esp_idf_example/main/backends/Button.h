#pragma once

// esp-idf
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
// OPEEngine
#include "Device.h"

class Button
{
    public:
        Button(Device& d, gpio_num_t button_pin);

    private:
        Device& d;

        gpio_num_t pin = GPIO_NUM_NC;

        TaskHandle_t scan_task_hdl;
        static void scan_task_trampoline(void* arg);
        void scan_task();

        static void IRAM_ATTR button_handler(void* arg);

        static const constexpr uint16_t SCAN_TASK_PRIO = 2;
        static const constexpr char* TAG = "Button";
};