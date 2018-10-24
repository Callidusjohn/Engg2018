#include <Arduino.h>
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
//#include <Serial.h>

static struct BluetoothUno {

  // create a connection between Uno and Mega
  void initiateConnToMega();

  // transfer data to Mega
  void transmitToMega(String data);

  // encrypt data using ROT-18 (ext of ROT-13)
  String encryptData(String data);

  // add a checksum to data for transmission
  int addChecksum(String message);

  // calculate checksum
  boolean calcChecksum(String message);

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
