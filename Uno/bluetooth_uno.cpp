#include <Arduino.h>
#include <AltSoftSerial.h>
#include <string.h>
#include "bluetooth_uno.h"

// IMPORTANT SET UP-INSTRUCTIONS FOR BLE CHIP
//  * VCC -> 5V
//  * GRD -> GRD
//  * PIN 8 -> TX
//  * PIN 9 -> RX

void BluetoothUno::initiateConnToMega() {
	//Serial.begin(9600); // don't need this bc Uno.ino defines it
	Serial.print("File:   ");
	Serial.println(__FILE__);
	Serial.print("Uploaded: ");
	Serial.println(__DATE__);
	Serial.println(" ");

	BTSerial.begin(9600);
	//BTSerial.write("AT+ROLE1");
	//BTSerial.write("AT+COND43639D8A14C"); // Mega address: D43639D8A14C
	Serial.println("Arduino Uno: Bluetooth Serial started at 9600 Baud.");
	//transmitToMega("Connection to Uno has been established.");
}

String BluetoothUno::getData() {
    if (BTSerial.available()) {
		String temp = "";
	    while (BTSerial.available()) {
			delay(10);
			char c = BTSerial.read();
			temp.concat(c);
	  	};
	    temp = encryptData(temp);
		Serial.print("RECEIVED: ");
		Serial.print(temp + "\n");
		if (!calcChecksum(temp)) {
			return "Anything"; // fix this to be error code
		}
  	};
	return "";
}

void BluetoothUno::transmitToMega(String data) {
	data = addChecksum(data);
	data = encryptData(data);
	for (int i = 0; i < data.length(); i++) {
		Serial.print(data[i]);
		BTSerial.write(data[i]);
	};
}

// encrypt data using variation of rot-13
// call this again to decrypt
// any chars should only be uppercase
String BluetoothUno::encryptData(String data) {
	String ROT18Msg = data;
	for (int i = 0; i < data.length(); i++) {
		// NOTE: assume upper case; message[i] = toupper(message[i]);
		char c = data[i];
		if (c > 47 && c < 58) {
			c += 25;
		} else {
			c += 18;
		};
		if (c > 90) {
			c -= 43;
		};
		if (c > 57 && c < 65) {
			c -= 7;
		};
		ROT18Msg[i] = c;
	}
	return ROT18Msg;
}

// add a checksum for even parity
String BluetoothUno::addChecksum(String message) {
	int sum = 0;
	String sumChar = "1";
	for (int i = 0; i < message.length(); i++) {
		char c = message[i];
		sum += c % 2;
	}
	if (sum % 2 == 0) {
		sumChar = "0";
	}
	String messageOut = message;
	messageOut += (sumChar);
	return messageOut;
}

// checksum calc to check the message for even parity
boolean BluetoothUno::calcChecksum(String message) {
	int sum = 0;
	for (size_t i = 0; i < message.length(); i++) {
		char c = message[i];
		sum += static_cast<int>(c) & 1;
	};
	return !(sum & 1);
}

// ****************************************
// * TEST FUNCTIONS                       *
// ****************************************

// this fnction allows transfer using serial monitor
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
	    if (NL) {
	    	Serial.print("\r>");
	    	NL = false;
	    }
	    Serial.write(c);
	    if (c == 10) {
			NL = true;
	    }
	}
}
