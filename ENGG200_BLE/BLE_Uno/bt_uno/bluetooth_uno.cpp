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
	// need some flag to ensure this isnt infinite
	BTSerial.write(data);
}

// encrypt data using variation of rot-13
// call this again to decrypt
string Bluetooth::encryptData(string data) {
	string ROT18Msg = data;
	for (int i = 0; i < message.length(); i++) {
		// NOTE: we can assume upper case! message[i] = toupper(message[i]);
		char c = message[i];
		if (c > 47 && c < 58) {
			c += 25;
		} else {
			c += 18;
		}
		if (c > 90) {
			c -= 43;
		}
		if (c > 57 && c < 65) {
			c -= 7;
		} ROT18Msg[i] = c;
	}
	return ROT18Msg;
}
