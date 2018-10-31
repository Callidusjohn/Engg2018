#define USE_MAGNET_ARM false

#include <Servo.h>
#include <Chrono.h>
//#include <HardwareSerial.h>
#include <AutoPID.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
//#include <AltSoftSerial.h>
#include "shared_utils.h"
#include "shared_types.h"
#include "MockAsync/async_handler.h"
#include "CanIntake/can_intake.h"
#include "Drive/motor_drive.h"
#include "Bluetooth/bluetooth_mega.h"


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
	//AsyncHandler.addCallback(&MotorDrive::updatePIDValues);

	//bluetooth added first
	// bluetooth inits CanIntake
	// bluetooth adds MotorDrive callback
	//	motor driveSomewhere() registers a callback for updatePIDValues which drives
	//  when motor gets to somewhere, adds callback for CanIntake
	AsyncHandler.addCallback(BluetoothMega::loopHook);
}

// Add the main program code into the continuous loop() function
void loop() {
	AsyncHandler.processLoop(); // goes at top
	// any other stuff that needs to happen
}