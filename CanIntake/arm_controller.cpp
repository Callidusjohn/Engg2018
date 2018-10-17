#include "arm_controller.h"
#if USE_MAGNET_ARM
#include "magnet_impl.cpp"
#else
#include "flipper_impl.cpp"
#endif

// when the last loophook runs, the quantity is subtracted from the remaining cans

void CanIntake::ArmController::loopHook() {
	return;
}

void CanIntake::ArmController::beginCollection(uint8_t cans) {

}