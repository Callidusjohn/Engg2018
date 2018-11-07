#include "arm_controller.h"
#include "shared_utils.h"
#include "async_handler.h"
#include "motor_drive.h"
#ifndef USE_MAGNET_ARM
#define USE_MAGNET_ARM false
#endif

bool CanIntake::ArmController::is_retracting;
bool CanIntake::ArmController::is_collecting;
uint8_t CanIntake::ArmController::quantity_to_collect;
uint8_t CanIntake::ArmController::last_collected_quantity;
bool CanIntake::ArmController::last_row_exhausted;
//
//void impl_init();
//void impl_reset();
//bool impl_shouldRetract();
//void impl_beforeRetracting();
//void impl_afterRetracting();

void CanIntake::ArmController::reset() {
	quantity_to_collect = 0;
	is_collecting = false;
	is_retracting = false;
	last_row_exhausted = false;
	impl_reset();
}

void CanIntake::ArmController::beginCollection(uint8_t cans) {
	reset();
	quantity_to_collect = cans;
	is_collecting = true;
	AsyncHandler.addCallback(extendArm, 500); // TODO: determine good delay, probably per arm (meaning this would go in an impl file)
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

// this constant won't change:
static constexpr int maxCount = 4;
static constexpr int rev = 200;       // 200 steps per revolution
static constexpr int revolutions_to_end = 380 / 5; // revs TO END: linear bearring 380mm, extension per rev 5mm

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
static int current_revolutions;

void CanIntake::ArmController::init() {
	// put your setup code here, to run once:
	// Sets the two pins as Outputs
	pinMode(Pins::arm_motor_stepper, OUTPUT);
	pinMode(Pins::arm_motor_direction, OUTPUT);
	pinMode(Pins::arm_limit_button, INPUT_PULLUP);
	pinMode(Pins::arm_start_button, INPUT_PULLUP);
	impl_init();

}


int motorOneRevExt() {
	digitalWrite(Pins::arm_motor_direction, LOW); // Sets direction of rotation to EXTEND the arm
	// Sends a Square Wave to the stepper pin, where for period results in a step
	// on the motor
	for (int x = 0; x < rev; x++) {
		digitalWrite(Pins::arm_motor_stepper, HIGH);
		delayMicroseconds(500);
		digitalWrite(Pins::arm_motor_stepper, LOW);
		delayMicroseconds(500);
	}
	return ++current_revolutions;
	//delay(1000); // Delay can be altered or removed
}

int motorOneRevRetract() {
	digitalWrite(Pins::arm_motor_direction, HIGH); // Sets direction of rotation to RETRACT the arm
	// Sends a Square Wave to the stepper pin, where for period results in a step
	// on the motor
	for (int x = 0; x < rev; x++) {
		digitalWrite(Pins::arm_motor_stepper, HIGH);
		delayMicroseconds(500);
		digitalWrite(Pins::arm_motor_stepper, LOW);
		delayMicroseconds(500);
	}
	return --current_revolutions;
}

inline bool checkLimitButton() {
	return digitalRead(Pins::arm_limit_button) == LOW;
}

void CanIntake::ArmController::extendArm() {
	//delay(500);
	if (!impl_shouldRetract() && current_revolutions < revolutions_to_end) {
		motorOneRevExt();
		AsyncHandler.addCallback(extendArm);
	}
	else {
		is_retracting = true;
		impl_beforeRetracting();
		AsyncHandler.addCallback(retractArm, 500);
	}
}

void CanIntake::ArmController::retractArm() {
	if (current_revolutions > 0) {
		motorOneRevRetract();
		AsyncHandler.addCallback(retractArm);
	}
	else {
		impl_afterRetracting();
	}
}


#if USE_MAGNET_ARM
#include "ultrasonic_sensor.h"
namespace MagnetArm {
	static int can_count = 0;

	static UltraSonicSensor sensor = UltraSonicSensor();

	static void init() {
		pinMode(Pins::magnet_trigger, OUTPUT);
		pinMode(Pins::magnet_button, INPUT);
		digitalWrite(Pins::magnet_trigger, LOW);
	}

	static void toggleMagnets(bool on) {
		if (on) {
			digitalWrite(Pins::magnet_trigger, HIGH);
		}
		else if (digitalRead(Pins::magnet_button) == 0) {
			digitalWrite(Pins::magnet_trigger, LOW);
		}
	};
}

int CanIntake::ArmController::checkCount() {
	return MagnetArm::sensor.sample();
}

void CanIntake::ArmController::impl_init() {
	MagnetArm::sensor.init();
}

void CanIntake::ArmController::impl_reset() {
	MagnetArm::sensor.reset();
	MagnetArm::can_count = 0;
}

bool CanIntake::ArmController::impl_shouldRetract() {
	int count = checkCount();
	bool limit_triggered = checkLimitButton();
	if (limit_triggered || count >= maxCount || count == quantity_to_collect || count > can_count) {
		can_count = count;
		return true;
	}
	return false;
}

void CanIntake::ArmController::impl_beforeRetracting() {
	bool limit_triggered = checkLimitButton();
	last_row_exhausted = limit_triggered || checkCount() >= maxCount;
	toggleMagnets(limit_triggered);
}

void CanIntake::ArmController::impl_afterRetracting() {
	toggleMagnets(false);
	if (checkCount() < quantity_to_collect && !last_row_exhausted) {
		AsyncHandler.addCallback(extendArm, 500);
	}
	else {
		remainingCans.quantityOf(currentlyCollecting) -= last_collected_quantity;
		AsyncHandler.addCallback(MotorDrive::driveSomewhere);
	}
}

#else
namespace FlipperArm {
	static int can_count = 0;
};
int CanIntake::ArmController::checkCount() {
	static int lastStateVal = 1;
	int sensorVal1 = digitalRead(Pins::flipper_counter_switch);
	if (sensorVal1 != lastStateVal) {
		if (sensorVal1 == HIGH) { //counts when sensor goes from LOW to HIGH
			FlipperArm::can_count++;
		}
	}
	lastStateVal = sensorVal1;
	return FlipperArm::can_count;
}

void CanIntake::ArmController::impl_init() {
	pinMode(Pins::flipper_counter_switch, INPUT_PULLUP);
}

void CanIntake::ArmController::impl_reset() {
	FlipperArm::can_count = 0;
}

bool CanIntake::ArmController::impl_shouldRetract() {
	int count = checkCount();
	bool limit_triggered = checkLimitButton();
	return limit_triggered || count >= maxCount || count == quantity_to_collect;
}

void CanIntake::ArmController::impl_beforeRetracting() {
	last_row_exhausted = checkLimitButton() || checkCount() >= maxCount;
}

void CanIntake::ArmController::impl_afterRetracting() {
	last_collected_quantity = checkCount();
	is_collecting = false;
	is_retracting = false;
	remainingCans.quantityOf(currentlyCollecting) -= last_collected_quantity;
	AsyncHandler.addCallback(MotorDrive::driveSomewhere);
}


#endif