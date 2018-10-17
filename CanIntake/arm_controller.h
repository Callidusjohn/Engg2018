#pragma once
#ifndef ARM_CONTROLLER_H
#define ARM_CONTROLLER_H
#include <Arduino.h>
#include "../shared_types.h"
#include "can_intake.h"

struct CanIntake::ArmController {
	bool isCollecting;
	bool lastRowExhausted;
	uint8_t lastCollectedQuantity;
	void loopHook();
	void beginCollection(uint8_t cans);
};

#endif