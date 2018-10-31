#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>
#include "bluetooth_mega.h"
#include "../shared_types.h"
#include "../MockAsync/async_handler.h"

bool BluetoothMega::NL = true;
char BluetoothMega::c = ' ';

void BluetoothMega::loopHook() {
	if (Serial2.available()) {
		String data = getData();
		CanQuantities cans = inputData(data);
		Serial.println(cans.red);
		Serial.println(cans.green);
		Serial.println(cans.blue);
	}
	AsyncHandler.addCallback(&loopHook);
}

BluetoothMega::BluetoothMega() {
	Serial.begin(9600);
	Serial2.begin(9600);
	Serial.print("File:   ");
	Serial.println(__FILE__);
	Serial.print("Uploaded: ");
	Serial.println(__DATE__);
	Serial.println(" ");
	// maybe force a disconnect and reconnect?
	// even reset to master/slave?
	Serial2.write("AT+CONXXXXXX"); // connect to bt chip, replace X with addr
	Serial.println("Arduino Mega: Bluetooth Serial started at 9600 Baud.");
	Serial2.print("Connection to Mega has been established.");
}

// ##### TODO: TEST THIS #####
String BluetoothMega::getData() {
	String temp = "";
	while (Serial2.available()) {
		char c = Serial2.read();
		temp.concat(c);
		delay(100);
	}
	if (temp != "") {
		// boolean check = calcChecksum(temp);
		// BTSerial.write(check);
		// delay(250);
		//if (BTSerial.read() == check) {
		temp = encryptData(temp);
		temp = encryptData(temp);
		//}
	} return temp; // change to error code for no information received
}

// this function allows transfer using serial monitor
void BluetoothMega::getInfo() {
	if (Serial2.available() > 0) {
		c = Serial2.read();
		Serial.write(c);
	}
	while (Serial.available() > 0) {
		c = Serial.read();

		if (c != 10 && c != 13) {
			Serial2.write(c);
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

void BluetoothMega::transmitToUno(String data) {
	// need some flag to ensure this isnt infinite
	for (int i = 0; i < data.length(); i++) {
		Serial2.write(data[i]);
	}
}

CanQuantities BluetoothMega::inputData(String temp) {
	return CanQuantities{ temp[1] - '0', temp[2] - '0', temp[3] - '0' };
}

// encrypt data using variation of rot-13
// call this again to decrypt
// any chars should only be uppercase
String BluetoothMega::encryptData(String data) {
	String ROT18Msg = data;
	for (int i = 0; i < data.length(); i++) {
		// NOTE: assume upper case; message[i] = toupper(message[i]);
		char c = data[i];
		if (c > 47 && c < 58) {
			c += 25;
		}
		else {
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

//check the message for even parity
boolean calcChecksum(String message) {
	int sum = 0;
	for (int i = 0; i < message.length(); i++) {
		char c = message[i];
		sum += c % 2;
	}
	if (sum % 2 == 0) {
		return true;
	}
	return false;
}

//add a checksum for even parity
String addChecksum(String message) {
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
