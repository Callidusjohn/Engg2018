#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>
#include "bluetooth_mega.h"
#include "shared_types.h"
#include "async_handler.h"
#include "can_intake.h"
#include "motor_drive.h"


void BluetoothMega::loopHook() {
	if (Serial2.available()) {
		String data = getData();
		Serial.print("\n");
		Serial.print("Data received: ");
		Serial.print(data);
		if (data[0] == '0' && data.length() == 5) {
			CanQuantities cans;
			cans = inputData(data);
			Serial.print("\n");
			Serial.print("Whole message: ");
			Serial.println(data);
			Serial.print("Red cans: ");
			Serial.println(cans.red);
			Serial.print("Green cans: ");
			Serial.println(cans.green);
			Serial.print("Blue cans: ");
			Serial.println(cans.blue);
			if (!CanIntake::needsMoreCans() && !MotorDrive::tripInProgress()) {
				CanIntake::setQuantities(cans);
				AsyncHandler.addCallback(MotorDrive::driveSomewhere);
			}
		};
	};
	AsyncHandler.addCallback(BluetoothMega::loopHook, 100);
}

void BluetoothMega::init() {
	Serial.begin(9600);
	Serial2.begin(9600);
	Serial.print("File:   ");
	Serial.println(__FILE__);
	Serial.print("Uploaded: ");
	Serial.println(__DATE__);
	Serial.println(" ");
	if (!(Serial2.available() || Serial2.availableForWrite())) {
		Serial2.write("AT+ROLE0");
		Serial2.write("AT+COND43639D8AE8A"); // Uno address: D43639D8AE8A
	}
	Serial.println("Arduino Mega: Bluetooth Serial started at 9600 Baud.");
	//Serial2.print(encryptData("Connection to Mega has been established."));
}

String BluetoothMega::getData() {
	String temp = "";
	if (Serial2.available()) {
		while (Serial2.available()) {
			delay(20);
			char c = Serial2.read();
			temp.concat(c);
		};
		temp = encryptData(temp);
		if (!calcChecksum(temp)) {
			Serial.println("Something is " + temp);
			return temp; // fix this to be error code
		};
	};
	return temp;
}

void BluetoothMega::transmitToUno(String data) {
	data = addChecksum(data);
	data = encryptData(data);
	for (size_t i = 0; i < data.length(); i++) {
		Serial2.write(data[i]);
	};
}

CanQuantities BluetoothMega::inputData(const String& temp) {
	constexpr uint8_t zero_char = '0';
	uint8_t r = temp[1] - zero_char;
	uint8_t g = temp[2] - zero_char;
	uint8_t b = temp[3] - zero_char;
	return CanQuantities{ r, g, b };
}

// encrypt data using variation of rot-13
// call this again to decrypt
// any chars should only be uppercase
String BluetoothMega::encryptData(String data) {
	String ROT18Msg = data;
	for (size_t i = 0; i < data.length(); i++) {
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
String BluetoothMega::addChecksum(String message) {
	int sum = 0;
	String sumChar = "1";
	for (size_t i = 0; i < message.length(); i++) {
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
bool BluetoothMega::calcChecksum(const String& message) {
	int sum = 0;
	for (size_t i = 0; i < message.length(); i++) {
		char c = message[i];
		sum += static_cast<int>(c) & 1;
	};
	return !(sum & 1);
}
