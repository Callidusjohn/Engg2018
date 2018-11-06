#pragma once
#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H
#include <Arduino.h>
#include <Chrono.h>
#include "shared_types.h"
// commonly re-used cross-module code signatures
#include <stdint.h>

static constexpr chrono_t chrono_t_max = ~(chrono_t)0;
static constexpr millis_t millis_t_max = ~(millis_t)0;

//TODO: Confirm these are the right pins
enum Pins : uint8_t { // not the *best* way to manage these, but it is simple and hard to break
	arm_motor_stepper = 3,
	arm_motor_direction = 4,
	arm_limit_button = 6,
	flipper_counter_switch = 7,
	arm_start_button = 8,
	ultrasonic_echo = 12,
	ultrasonic_trigger = 13,
	magnet_trigger = 43,
	magnet_button = 44,
	drive_forward_sensor = 54,
	drive_reverse_sensor = 55,
	drive_rgb_s3 = 55, // was 6 in original code
	drive_rgb_s2 = 56, // was 7 in original code
	drive_rgb_s1 = 57,
	drive_rgb_s0 = 58,
	drive_rgb_sensorOut = 59, // was 8 in original code
	drive_ir_leftSensor = 22,
	drive_ir_rightSensor = 23,
	drive_ir_leftInterrupt = 18,
	drive_ir_rightInterrupt = 19,

};

#endif