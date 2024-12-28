#include "Button.h"

Button::Button(Device& d, gpio_num_t button_pin)
    : d(d)
    , pin(button_pin)
{

    gpio_config_t gpio_conf;

    gpio_conf.intr_type = GPIO_INTR_NEGEDGE;
    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.pin_bit_mask = (1ULL << pin);
    gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&gpio_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(pin, button_handler, (void*) this);

    xTaskCreate(scan_task_trampoline, "BtnScanTsk", 2048, this, SCAN_TASK_PRIO, &scan_task_hdl);
}

void Button::scan_task_trampoline(void* arg)
{
    Button* button = static_cast<Button*>(arg);
    button->scan_task();
}

void Button::scan_task()
{
    static const constexpr TickType_t POLLING_PERIOD_MS = 50UL / portTICK_PERIOD_MS;
    int poll_count = 0;

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // block until notified by respective GPIO ISR
        vTaskDelay(POLLING_PERIOD_MS);           // debounce press

        do
        {
            poll_count++;

            if (poll_count == 3)
                d.usr_button.set(button_event_t::long_press);
            else if (poll_count >= 4)
                d.usr_button.set(button_event_t::held);

            vTaskDelay(POLLING_PERIOD_MS);

        } while (gpio_get_level(pin) == 0);

        if (poll_count <= 2)
            d.usr_button.set(button_event_t::quick_press);

        d.usr_button.set(button_event_t::release);

        vTaskDelay(POLLING_PERIOD_MS); // debounce release

        poll_count = 0;

        gpio_intr_enable(pin);
    }
}

void IRAM_ATTR Button::button_handler(void* arg)
{
    BaseType_t xHighPriorityTaskWoken = pdFALSE;
    Button* button = (Button*) arg;
    gpio_intr_disable(button->pin); // disable interrupt until re-enabled in button task

    vTaskNotifyGiveFromISR(button->scan_task_hdl, &xHighPriorityTaskWoken); // notify button task button input was detected
    portYIELD_FROM_ISR(xHighPriorityTaskWoken);                             // perform context switch if necessary
}