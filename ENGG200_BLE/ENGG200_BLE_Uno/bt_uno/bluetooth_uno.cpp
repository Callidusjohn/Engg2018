#include <Arduino.h>
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include <String.h>
#include "bluetooth_uno.h"


void BluetoothUno::initiateConnToMega() {
	Serial.begin(9600);
	Serial.print("File:   ");
	Serial.println(__FILE__);
	Serial.print("Uploaded: ");
	Serial.println(__DATE__);
	Serial.println(" ");

	BTSerial.begin(9600);
	Serial.println("Arduino Uno: Bluetooth Serial started at 9600 Baud.");
	BTSerial.print("Connection to Uno has been established.");
}

// this function allows transfer using serial monitor
void BluetoothUno::getInfo() {
	if (BTSerial.available() > 0) {
		c = BTSerial.read();
		Serial.write(c);
	}
	while (Serial.available() > 0) {
		c = Serial.read();

		if (c != 10 && c != 13) {
			BTSerial.write(c);
		}

		if (NL) { Serial.print("\r>");  NL = false; }
		Serial.write(c);
		if (c == 10) { NL = true; }
	}
}

void BluetoothUno::transmitToMega(int data) {
	BTSerial.write(data);
	if (NL) {
		Serial.print("\r>");
		NL = false;
	}
	Serial.write(data);
	if (data == 10) {
		NL = true;
	}
}

void BluetoothUno::transferToMega(int data) {
	if (Serial.available()) {
		Serial.write(data);
	}

	// fix this up?
	if (data != '\r' && data != '\n') {
		Serial.write(data);
	}
}
