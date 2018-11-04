#ifndef BLUETOOTH_MEGA_H
#define BLUETOOTH_MEGA_H
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "shared_types.h"

struct BluetoothMega {

	static void loopHook();
	static void init();

	// BluetoothMega();

	// get data sent and return string form
	static String getData();

	// transfer data to Mega
	static void transmitToUno(const String& data);

	// encrypt data using ROT-18 (ext of ROT-13)
	static String encryptData(const String& data);

	// calculate checksum
	static bool calcChecksum(const String& message);

	// adds checksum to message
	static String addChecksum(String message);

	// return can quantities in struct for other teams
	static CanQuantities inputData(const String& data);
};
#endif
