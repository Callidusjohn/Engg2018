#ifndef BLUETOOTH_MEGA_H
#define BLUETOOTH_MEGA_H
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "shared_types.h"
//#include <Serial.h>

static struct BluetoothMega {

	static void loopHook();
	void initBluetoothConnection();

	BluetoothMega();

	// get data sent and return string form
	static String getData();

	// transfer data to Mega
	static void transmitToUno(String data);

	// encrypt data using ROT-18 (ext of ROT-13)
	static String encryptData(String data);

	// add a checksum to data for transmission
	static int addChecksum(String message);

	// calculate checksum
	static boolean calcChecksum(String message);

	static void transmitToMega(int data);

	static void lcd_errorChecking(int data);

	static void getInfo();

	static CanQuantities quantity;

	static CanQuantities inputData(String data);

	static bool NL;
	static char c;
}BluetoothMega;
#endif