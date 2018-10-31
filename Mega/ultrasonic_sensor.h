#pragma once
#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H
#include <Chrono.h>
#include <Arduino.h>
class UltraSonicSensor {
public:

	UltraSonicSensor();

	int checkDistance(); // Maybe
	int sample();
	void reset();

private:
	static constexpr size_t sample_buffer_size = 12;

	size_t index;
	long samples[sample_buffer_size]; // cyclic buffer vs fixed calculation?
	int count;
	int currentState;
	int previousState;
	bool ascending = true;
	Chrono::chrono_t delay_until;

	Chrono chrono;
};
#endif