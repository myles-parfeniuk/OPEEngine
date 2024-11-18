#include "FakeLED.h"

FakeLED::FakeLED(Device& d):
	d(d)
{
	//grab initial values from device model
	current_pwm = d.led.pwm_percentage.get();
	state = d.led.state.get(); 

	//subscribe (register callbacks) to relevant data in device model
	d.usr_button.subscribe<32>([this, &d](button_event_t event) {
		
		/*this code will be executed whenver a usr_button is set() within the device model*/
		
		switch (event)
		{
		case button_event_t::quick_press:

			current_pwm += 10;
			if (current_pwm > 100)
				current_pwm = 0;

			d.led.pwm_percentage.set(current_pwm);

			printf("%s: callback: SET PWM: %d%%\n\r", TAG, current_pwm);
			break;

		case button_event_t::long_press:
			state = !state;  //toggle led
			d.led.state.set(state);

			printf("%s: callback: SET state: %s\n\r", TAG, state ? "on" : "off");

			break;

		case button_event_t::release:

			break;

		default:
			break; 
		}

	});



	//pwm peripheral setup would happen here
}