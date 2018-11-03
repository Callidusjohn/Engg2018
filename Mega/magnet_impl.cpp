#include "arm_controller.h"
#include "shared_utils.h"
#include "ultrasonic_sensor.h"

static UltraSonicSensor sensor = UltraSonicSensor();

struct MagnetArm {
	void init() {
		pinMode(Pins::magnet_trigger, OUTPUT);
		pinMode(Pins::magnet_button, INPUT);
		digitalWrite(Pins::magnet_trigger, LOW);
	}

	void toggleMagnets(bool on) {
		if (on) {
			digitalWrite(Pins::magnet_trigger, HIGH);
		}
		else if (digitalRead(Pins::magnet_button) == 0) {
			digitalWrite(Pins::magnet_trigger, LOW);
		}
	}
};