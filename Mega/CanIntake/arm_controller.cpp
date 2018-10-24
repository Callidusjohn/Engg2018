#include "arm_controller.h"
#if USE_MAGNET_ARM
#include "magnet_impl.cpp"
#else
#include "flipper_impl.cpp"
#endif
#include "../MockAsync/async_handler.h"

void CanIntake::ArmController::beginCollection(uint8_t cans) {
	quantity_to_collect = cans;
	is_collecting = true;
	//auto t = (this->continueCollection);
	AsyncHandler.addCallback(&continueCollection); // TODO: determine good delay, probably per arm (meaning this would go in an impl file)
	// delay could potentially stay zero due to the implementation of the async handler
}

bool CanIntake::ArmController::isCollecting() noexcept {
	return is_collecting;
}

bool CanIntake::ArmController::lastRowExhausted() noexcept {
	return last_row_exhausted;
}

uint8_t CanIntake::ArmController::lastCollectedQuantity() noexcept {
	return last_collected_quantity;
}