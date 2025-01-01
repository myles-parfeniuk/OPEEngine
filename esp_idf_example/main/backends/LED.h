#pragma once
// std lib
#include <cstdio>
// esp-idf
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
// OPEEngine
#include "Device.h"

class LED
{
    public:
        LED(Device& d, gpio_num_t led_pin);

    private:
        Device& d;
        gpio_num_t pin = GPIO_NUM_NC;
        uint8_t current_pwm_pct;
        bool state;

        void set_pwm(uint8_t new_pwm_pct);

        static const constexpr char* TAG = "LED";
};