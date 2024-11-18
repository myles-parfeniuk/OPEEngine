#include "FakeButton.h"

#include <random>

#define CHANCE_USER_PRESS_BUTTON(gen) (std::uniform_int_distribution<>(1, 3)(gen) == 1)
#define CHANCE_USER_RELEASE_BUTTON(gen) (std::uniform_int_distribution<>(1, 2)(gen) == 1)

FakeButton::FakeButton(Device& d) :
	d(d)
{
	xTaskCreate(
		scan_task_trampoline,       
		"BtnScanTsk",          
		128,      
		this,    
		SCAN_TASK_PRIO,
		&scan_task_hdl);      

}

void FakeButton::scan_task_trampoline(void* arg)
{
	FakeButton* button = static_cast<FakeButton*>(arg);
	button->scan_task(); 
}

void FakeButton::scan_task()
{

	//random number gnerator for simulated button presses
	std::random_device rng_device;
	std::mt19937 rng(rng_device());

	/*logic to simulate button presses based off random chance, doesn't represent reality
	 key take away is that set would be called in the codebase where button presses are detected*/

	int poll_count = 0; 
	bool long_press = false; 

	while (1)
	{
		if (CHANCE_USER_PRESS_BUTTON(rng))
		{
			vTaskDelay(20 / portTICK_PERIOD_MS); //fake debounce

			while (!CHANCE_USER_RELEASE_BUTTON(rng))
			{
				
				//wait for release
				vTaskDelay(15 / portTICK_PERIOD_MS);

				if (poll_count > 1 && !long_press)
				{
					printf("%s: SET long-press event\n\r", TAG);
					d.usr_button.set(button_event_t::long_press);
					long_press = true;
				}

				poll_count++;
			}

			vTaskDelay(20 / portTICK_PERIOD_MS); //fake debounce

			if (poll_count > 1)
			{
				printf("%s: SET release event\n\r", TAG);
				d.usr_button.set(button_event_t::release);
				
			}
			else
			{
				printf("%s: SET quick-press event\n\r", TAG);
				d.usr_button.set(button_event_t::quick_press);
				
			}

			poll_count = 0;
			long_press = false; 

		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}