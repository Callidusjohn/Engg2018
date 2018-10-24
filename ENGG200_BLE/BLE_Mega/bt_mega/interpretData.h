#include <Arduino.h>
#include "shared_types.h"
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>

static struct InterpretData {
	char interpretRed(char c, char r);
	char interpretGreen(char c, char g);
	char interpretBlue(char c, char b);

	CanQuantities inputData();

	char r = '/';
	char g = '/';
	char b = '/';
	
} InterpretData;
