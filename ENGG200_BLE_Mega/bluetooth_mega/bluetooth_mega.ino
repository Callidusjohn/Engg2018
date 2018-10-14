#include <SoftwareSerial.h>

bool initiateConnToUno() {
	int PIN = 9;
	pinMode(PIN, INPUT);
	Serial.begin(9600);
	Serial.print("AT"); // this should print OK to monitor
	Serial.write("AT+nnnnnnnn"); // replace n with MAC address of Uno
}

void receiveDataFromUno() {
	char c; // replace with data type

	if (Serial.available()) {
		c = Serial.read();
		Serial.write(c);
	}
	if (c > 0) {
		// call other functions here
	}
}
