#pragma once
#ifndef CAN_INTAKE_H
#define CAN_INTAKE_H
#include <Arduino.h>
#include "shared_types.h"
//#include "arm_controller.h"

struct CanIntake {
	friend class ArmController;

	// initializes the internal state object for the arm
	static void initState(const CanQuantities& cans);

	// true if insufficient cans have been collected
	static bool needsMoreCans() noexcept;

	// true if insufficient cans of a given type have been collected
	static bool needsMoreCans(CanType type) noexcept;

	// starts asynchronous can collection
	static void beginCollection(CanType type);

	// checks if the arm is busy collecting
	static bool isCollecting() noexcept;

	// checks if the last row the arm fed from was exhausted
	static bool lastRowExhausted() noexcept;

	static uint8_t lastCollectedQuantity() noexcept;
	
private:

	class ArmController;

	//static ArmController armController;
	static CanQuantities remainingCans;
	static CanType currentlyCollecting;

	static void updateQuantites() noexcept;
};
#endif