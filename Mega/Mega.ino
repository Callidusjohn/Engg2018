#define USE_MAGNET_ARM false
#include <Servo.h>
#include <Chrono.h>
#include <AutoPID.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "shared_utils.h"
#include "shared_types.h"
#include "async_handler.h"
#include "can_intake.h"
#include "motor_drive.h"
#include "bluetooth_mega.h"

/* Your code, if it does not go here or in a shared file, goes in a namespace or static module object.
 * Prefer namespaces to static objects when possible, as at best a well-written static object is
 * as efficient as a namespace but in general will use more memory and storage.
 */

 // The setup() function runs once each time the micro-controller starts
void setup() {
	// initialize here

	// call AsyncHandler.addCallback(&function, delay) here to register your initial execution
	// each function may only have ONE delayed callback registered at a time
	// called functions can register other functions and themselves
	//AsyncHandler.addCallback(&MotorDrive::driveLoop);
	BluetoothMega::init();
	MotorDrive::init();
	CanIntake::init();


	//bluetooth added first
	// bluetooth inits CanIntake
	// bluetooth adds MotorDrive callback
	//	motor driveSomewhere() registers a callback for driveLoop which drives
	//  when motor gets to somewhere, adds callback for CanIntake
	AsyncHandler.addCallback(BluetoothMega::loopHook);
	//CanQuantities cans = { 1, 0, 0 };
	//CanIntake::setQuantities(cans);
	//CanIntake::beginCollection(CanType::red);
}

// Add the main program code into the continuous loop() function
void loop() {
	AsyncHandler.processLoop(); // goes at top
	// any other stuff that needs to happen
}
