#include <SoftwareSerial.h>
#include <bluetooth_uno.h>

bool initiateConnToMega() {
	int PIN = 9;
	pinMode(PIN, INPUT);
	Serial.begin(9600);
	Serial.write("AT+nnnnnnnn"); // replace n with MAC address
}

int transferToMega(int data) {
	if (Serial.available()) {
      Serial.write(data);
    }

	// fix this up?
	if (c != '\r' && c != '\n') {
		Serial.write(data);
	}
}
