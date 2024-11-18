#pragma once

//std lib includes
#include <cstdio>
//OPEEngine includes
#include "OPEEDevice.h"

class FakeButton
{
	public:
		FakeButton(Device& d);

	private:
		Device& d;

		TaskHandle_t scan_task_hdl;
		static void scan_task_trampoline(void* arg);
		void scan_task();

		static const constexpr uint16_t SCAN_TASK_PRIO = 2;
		static const constexpr char* TAG = "FakeButton";
};