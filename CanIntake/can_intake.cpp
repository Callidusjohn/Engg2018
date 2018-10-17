#include "can_intake.h"
#include "arm_controller.h"

void CanIntake::loopHook() {
	if (isCollecting()) armController.loopHook();
}

void CanIntake::initState(const CanQuantities& cans) {
	if (isCollecting()) {
		//TODO: log error here in debug if we end up doing logging
	}
	else {
		remainingCans = cans;
	}
}

bool CanIntake::needsMoreCans() noexcept {
	return remainingCans.blue != 0 || remainingCans.green != 0 || remainingCans.red != 0;
}

bool CanIntake::needsMoreCans(CanType type) noexcept {
	return remainingCans.quantityOf(type) != 0;
}

void CanIntake::beginCollection(CanType type) {
	currentlyCollecting = type;

	armController.beginCollection(remainingCans.quantityOf(type));
}

bool CanIntake::isCollecting() noexcept {
	//stub
	return armController.isCollecting;
}

bool CanIntake::lastRowExhausted() noexcept {
	return armController.lastRowExhausted;
}

uint8_t CanIntake::lastCollectedQuantity() noexcept {
	return armController.lastCollectedQuantity;
}

void CanIntake::updateQuantites() noexcept {
	remainingCans.quantityOf(currentlyCollecting) -= armController.lastCollectedQuantity;
}