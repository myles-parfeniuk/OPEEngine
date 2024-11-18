/*
	This application was based off of ihavn's MSVC-freeRTOS template, which can be found here:

	https://github.com/ihavn/MSVC-FreeRTOS-Template-202012.00

	It has been modified to function as a demonstration/simulator of OPEEngine.

	Note that while OPEEngine itself has no dynamic memory allocation, 
	the windows freeRTOS port as well as std::cout does.
	Therefore this demonstration involves dynamic memory allocation. 

*/

//std lib includes
#include <iostream>
#include <cstdio>
//freeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

//OPEEngine includes
#include "OPEEDevice.h"

//windows demo specific inclues
#include "FakeLED.h"
#include "FakeButton.h"
#include "FakeUART.h"


Device d;
FakeLED led(d);
FakeButton button(d);
FakeUART uart(d);

void test_task(void* arg);

// --------------------------------------------------------------------------------------
extern "C" int main(void)
{
	//let the operating system take over :)
	vTaskStartScheduler();
}