#include <Arduino.h>
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
//#include <Serial.h>

static struct BluetoothUno {

	// create a connection between Uno and Mega
	void initiateConnToMega();

	// encrypt data using ROT-18 (ext of ROT-13)
	int encryptData(int data);

	// add a checksum to data for transmission
	int addChecksum(int data);

	// calculate checksum
	int calcChecksum(int data);

	// transfer data to Mega
	void transferToMega(int data);

	void transmitToMega(int data);

	void lcd_errorChecking(int data);

	void getInfo();

	int PIN;
	AltSoftSerial BTSerial;
	bool NL = true;
	char c = ' ';
	struct cans {
		unsigned char red;
		unsigned char green;
		unsigned char blue;
	};
} BluetoothUno;
