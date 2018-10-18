#pragma once
#ifndef ARM_CONTROLLER_H
#define ARM_CONTROLLER_H
#include <Arduino.h>
#include "../shared_types.h"
#include "can_intake.h"

class CanIntake::ArmController {
	friend struct CanIntake;
	bool is_collecting;
	bool last_row_exhausted;
	uint8_t last_collected_quantity;
	void loopHook();
	void beginCollection(uint8_t cans);
};

#endif