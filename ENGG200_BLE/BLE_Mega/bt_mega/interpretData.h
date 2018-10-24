#include <Arduino.h>
#include "shared_types.h"

static struct InterpretData {
	char interpretRed(char c, char r);
	char interpretGreen(char c, char g);
	char interpretBlue(char c, char b);

	void inputData ();
} InterpretData;
