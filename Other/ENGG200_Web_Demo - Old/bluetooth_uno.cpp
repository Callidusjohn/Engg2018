#include <Arduino.h>
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include <string.h>
#include "bluetooth_uno.h"


void BluetoothUno::initiateConnToMega() {
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

void BluetoothUno::transmitToMega(String data) {
  // need some flag to ensure this isnt infinite
  for (int i = 0; i < data.length(); i++) {
    BTSerial.write(data[i]);
  }
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
  if (sum % 2 == 0){
    sumChar = "0";
  }
  String messageOut = message;
  messageOut += (sumChar);
  // cout << "message: " + messageOut + "\n";
  return messageOut;
}
