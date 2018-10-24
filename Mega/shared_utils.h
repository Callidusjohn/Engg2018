#pragma once
#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H
#include <Arduino.h>
#include <Chrono.h>
#include "shared_types.h"
// commonly re-used cross-module code signatures
#include <stdint.h>

static constexpr Chrono::chrono_t chrono_t_max = ~(Chrono::chrono_t)0;

enum Pins : uint8_t { // not the *best* way to manage these, but it is simple and hard to break
	ultrasonic_echo = 12,
	ultrasonic_trigger = 13
};

#endif