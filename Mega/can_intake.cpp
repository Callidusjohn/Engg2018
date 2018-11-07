#include "can_intake.h"
#include "arm_controller.h"

//CanIntake::ArmController CanIntake::armController = ArmController();
CanQuantities CanIntake::remainingCans = { 0, 0, 0 };
CanType CanIntake::currentlyCollecting;

void CanIntake::init() {
	ArmController::init();
}

void CanIntake::setQuantities(const CanQuantities& cans) {
	if (isCollecting()) {
		return;
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
	ArmController::beginCollection(remainingCans.quantityOf(type));
}

bool CanIntake::isCollecting() noexcept {
	return ArmController::isCollecting();
}

bool CanIntake::lastRowExhausted() noexcept {
	return ArmController::lastRowExhausted();
}

uint8_t CanIntake::lastCollectedQuantity() noexcept {
	return ArmController::lastCollectedQuantity();
}