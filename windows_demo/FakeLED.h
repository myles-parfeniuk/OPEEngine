#pragma once
//std lib includes
#include <cstdio>
//OPEEngine includes
#include "OPEEDevice.h"

class FakeLED
{
	public:
		FakeLED(Device& d);
	
	private:
		Device& d;

		uint8_t current_pwm;
		bool state; 

		static const constexpr char* TAG = "FakeLED";
};