#include "ultrasonic_sensor.h"
#include <Arduino.h>
#include "shared_utils.h"

Chrono UltraSonicSensor::chrono = Chrono(Chrono::MILLIS);

UltraSonicSensor::UltraSonicSensor() {
	pinMode(Pins::ultrasonic_echo, INPUT);
	pinMode(Pins::ultrasonic_trigger, OUTPUT);
	for (size_t i = 0; i < sample_buffer_size; i++) {
		samples[i] = 0;
	}
}

void UltraSonicSensor::reset() {
	count = 0;
}

int UltraSonicSensor::sample() {
	if (chrono.hasPassed(delay_until)) {
		long duration, distance;
		digitalWrite(Pins::ultrasonic_trigger, LOW);
		delayMicroseconds(2);
		digitalWrite(Pins::ultrasonic_trigger, HIGH);
		delayMicroseconds(10);
		digitalWrite(Pins::ultrasonic_trigger, LOW);
		duration = pulseIn(Pins::ultrasonic_echo, HIGH);
		delay_until = chrono.elapsed() + 50;
		distance = (duration * 3.4) / 2;
		if (distance <= 500) {    //distance < 5cm
			samples[index] = distance;
			index++;
			if (index >= sample_buffer_size) {   //array is full --> begin sampling
				int a, b, c = 0;
				for (size_t i = 0; i < 3; i++) { //split array into 3 sections, avg each section
					a += samples[i];
				}
				a /= 3;

				for (size_t i = 3; i < 6; i++) {
					b += samples[i];
				}
				b /= 3;

				for (size_t i = 6; i < sample_buffer_size; i++) {
					c += samples[i];
				}
				c /= 3;

				int result = c - b - a;   //difference between the avg of each section
				if (result > 0) {
					if (!ascending) {
						count++;
					}
					ascending = true;
				}
				else {
					ascending = false;
				}
				index = 0;
			}
		}
	}
	return count;
}


/*{
  sendSound(); // write pin HIGH then LOW after 10us

  int val = (pulseIn(echoPin, HIGH) / 2) / 29.1;
  if (val < 5) {
	samples[index] = val;
	index++;
	if (index == 10) {
	  index = 0;
	}
  } else {
	return
	  }

  return val;
}
*/