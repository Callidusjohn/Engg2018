#include <Arduino.h>
#include "shared_types.h"

static struct InterpretData {
	char interpretRed(char c);
	char interpretGreen(char c);
	char interpretBlue(char c);

	void inputData ();
}
