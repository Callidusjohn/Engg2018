#pragma once
#ifndef ARM_CONTROLLER_H
#define ARM_CONTROLLER_H
#include <Arduino.h>
#include "shared_types.h"
#include "can_intake.h"

struct CanIntake::ArmController {
public:
	static void init();
	static void beginCollection(uint8_t cans);
	static bool isCollecting() noexcept;
	static bool lastRowExhausted() noexcept;
	static uint8_t lastCollectedQuantity() noexcept;
private:
	static int checkCount();
	static void reset();
	//static void impl_reset();
	//static void impl_init();
	static void extendArm();
	static void retractArm();
	static bool is_collecting;
	static bool is_retracting;
	static bool last_row_exhausted;
	static uint8_t last_collected_quantity;
	static uint8_t quantity_to_collect;

};

#endif