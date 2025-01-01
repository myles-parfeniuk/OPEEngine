#include "LED.h"

LED::LED(Device& d, gpio_num_t led_pin)
    : d(d)
    , pin(led_pin)
{
    // grab initial values from device model
    current_pwm_pct = d.led.pwm_percentage.get();
    state = d.led.state.get();

    // subscribe (register callbacks) to relevant data in device model
    d.usr_button.subscribe<16>(
            [this, &d](button_event_t event)
            {
                /*this code will be executed whenver a usr_button is set() within the device model*/

                switch (event)
                {
                    case button_event_t::quick_press:

                        current_pwm_pct += 10;
                        if (current_pwm_pct > 100)
                            current_pwm_pct = 0;

                        if (state)
                            set_pwm(current_pwm_pct);

                        d.led.pwm_percentage.set(current_pwm_pct);

                        ESP_LOGI(TAG, "callback: SET PWM: %d%%", current_pwm_pct);
                        break;

                    case button_event_t::long_press:
                        state = !state; // toggle led
                        d.led.state.set(state);

                        if (state)
                            set_pwm(current_pwm_pct);
                        else
                            set_pwm(0U);

                        ESP_LOGI(TAG, "callback: SET state: %s", state ? "on" : "off");

                        break;

                    case button_event_t::release:

                        break;

                    default:
                        break;
                }
            });

    ledc_timer_config_t ledc_timer = {.speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = LEDC_TIMER_13_BIT,
            .timer_num = LEDC_TIMER_0,
            .freq_hz = 4000UL, // Set output frequency at 4 kHz
            .clk_cfg = LEDC_AUTO_CLK};

    ledc_channel_config_t ledc_channel = {.gpio_num = pin,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = LEDC_CHANNEL_0,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = LEDC_TIMER_0,
            .duty = 0, // Set duty to 0%
            .hpoint = 0};

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void LED::set_pwm(uint8_t new_pwm_pct)
{
    static const constexpr uint32_t MAX_PWM_VAL = 8192UL;
    uint32_t ledc_duty = static_cast<uint32_t>(static_cast<float>(MAX_PWM_VAL) * (static_cast<float>(new_pwm_pct) / 100.0f));

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, ledc_duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}