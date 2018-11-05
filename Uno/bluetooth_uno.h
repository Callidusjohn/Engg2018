#include <Arduino.h>
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
//#include <Serial.h>

static struct BluetoothUno {

	// create a connection between Uno and Mega
	void initiateConnToMega();

	// transfer data to Mega
 	void transmitToMega(String data); //Should use the above.

	// get data from the Mega
	String getData();

	// encrypt data using ROT-18 (ext of ROT-13)
	String encryptData(String data);

	// add a checksum to data for transmission
	String addChecksum(String message);

	// calculate checksum
	boolean calcChecksum(String message);

	int PIN;
	AltSoftSerial BTSerial;
	bool NL = true;
	char c = '';

	// ##########################
	// ##### TEST FUNCTIONS #####
	// ##########################

	void lcd_errorChecking(int data);

	void getInfo();

} BluetoothUno;
