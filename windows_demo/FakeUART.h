#pragma once
//std lib includes
#include <cstdio>
#include <cstring>
//OPEEngine includes
#include "OPEEDevice.h"

class FakeUART
{
public:
	FakeUART(Device& d);

private:
	Device& d;

	TaskHandle_t uart_task_hdl;
	static void uart_task_trampoline(void* arg);
	void uart_task();

	uint8_t tx_buffer[256];

	static const constexpr uint16_t UART_TASK_PRIO = 3;
	static const constexpr char* TAG = "FakeUART";
};