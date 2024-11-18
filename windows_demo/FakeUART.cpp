#include "FakeUART.h"

FakeUART::FakeUART(Device& d) :
	d(d)
{

	d.led.pwm_percentage.subscribe<32>([this, &d](uint8_t new_pwm) {
		sprintf_s(reinterpret_cast<char*>(tx_buffer), sizeof(tx_buffer), "callback: uart tx current pwm: %d%% new pwm: %d%%", d.led.pwm_percentage.get(), new_pwm);
		xTaskNotify(uart_task_hdl, 1, eSetValueWithOverwrite);
		});

	d.led.state.subscribe<24>([this](bool new_state)
		{
			sprintf_s(reinterpret_cast<char*>(tx_buffer), sizeof(tx_buffer), "callback: uart tx led state: %s", new_state ? "on" : "off");
			xTaskNotify(uart_task_hdl, 1, eSetValueWithOverwrite);
		});

	xTaskCreate(
		uart_task_trampoline,
		"UARTTask",
		128,
		this,
		UART_TASK_PRIO,
		&uart_task_hdl);



}

void FakeUART::uart_task_trampoline(void* arg)
{
	FakeUART* uart_service = static_cast<FakeUART*>(arg);
	uart_service->uart_task();
}

void FakeUART::uart_task()
{
	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		printf("%s: %s\r\n", TAG, tx_buffer);
	}
}