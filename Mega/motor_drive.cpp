#include "motor_drive.h"
#include "async_handler.h"
#include "can_intake.h"



//IR PID consts
static constexpr int whiteTapeFwd = 563;
static constexpr int blackTapeFwd = 981;
static constexpr int whiteTapeRwd = 570;
static constexpr int blackTapeRwd = 970;
static constexpr int currentPosition = 0;
static constexpr int error = 0;
static constexpr double integral = 0;
static constexpr double derivative = 0;
static constexpr double previousError = 0;
static constexpr int outputSpeed = 0;
static constexpr int mapLower = 90;
static constexpr int mapUpper = 90;

double MotorDrive::countLeft = 0;
double MotorDrive::countRight = 0;

int MotorDrive::dir = 1;

int MotorDrive::targetPos = 1000;

double MotorDrive::irRightAddition, MotorDrive::irLeftAddition, MotorDrive::driveLeftPidOutput, MotorDrive::driveRightPidOutput;
double MotorDrive::inputIR, MotorDrive::setPointIR, MotorDrive::outputFromIR;

//DRIVE PIDS
double MotorDrive::driveSetPoint;



//will come from sensor
CanType MotorDrive::detectedColor;

//will eventually come from state.
//CanType MotorDrive::color;


//rgb stuff
int MotorDrive::frequency, MotorDrive::redFreq, MotorDrive::greenFreq, MotorDrive::blueFreq;
bool MotorDrive::sensorRed, MotorDrive::sensorGreen, MotorDrive::sensorBlue;

// servos
Servo MotorDrive::servoLeft;
Servo MotorDrive::servoRight;

int MotorDrive::throttleLeft, MotorDrive::throttleRight;

bool MotorDrive::has_read_a_color;
bool MotorDrive::color_reading_in_progress;
millis_t MotorDrive::disable_color_sensor_until;

MotorDrive::MotorDrive()
{
	driveSetPoint = targetPos;
	attachInterrupt(digitalPinToInterrupt(Pins::drive_ir_leftInterrupt), ISRleft, RISING);
	attachInterrupt(digitalPinToInterrupt(Pins::drive_ir_rightInterrupt), ISRright, RISING);
	pinMode(Pins::drive_ir_leftSensor, INPUT); // 18 and 22 come from left
	pinMode(Pins::drive_ir_rightSensor, INPUT); // 19 and 23 come from right
	pinMode(Pins::drive_rgb_s3, INPUT);//pinMode(6, INPUT); //enable pin
	servoLeft.attach(4);  // attaches the servo on pin 9 to the servo object
	servoRight.attach(5);  // attaches the servo on pin 9 to the servo object
}

void MotorDrive::ISRleft() {
	if (digitalRead(Pins::drive_ir_leftSensor) == LOW) {
		countLeft = countLeft + 1;
	}
	else {
		countLeft = countLeft - 1;
	};
}
void MotorDrive::ISRright() {
	if (digitalRead(Pins::drive_ir_rightSensor) == HIGH) {
		countRight = countRight + 1;
	}
	else {
		countRight = countRight - 1;
	};
}

bool MotorDrive::inRange(int val, int minimum, int maximum) {
	return ((minimum <= val) && (val <= maximum));
}

void MotorDrive::driveSomewhere() {
	if (CanIntake::needsMoreCans()) {
		dir = 1;
		AsyncHandler.addCallback(&checkColorSensor);
	}
	else {
		dir = -1;
	};
	AsyncHandler.addCallback(&updatePIDValues, 100);
}

void MotorDrive::updatePIDValues() {
	addLinePidValues(dir);
	AsyncHandler.addCallback(&updatePIDValues, 100);
}

void MotorDrive::checkColorSensor() {
	if (color_reading_in_progress) return;
	millis_t time_now = millis(); // store to avoid overflow if the times are close
	if (time_now > disable_color_sensor_until) {
		color_reading_in_progress = true;
		checkColorSensorPhase1();
	}
	else {
		AsyncHandler.addCallback(&checkColorSensor, disable_color_sensor_until - time_now);
	}

}

void MotorDrive::checkColorSensorPhase1() {
	digitalWrite(Pins::drive_rgb_s2, LOW);
	digitalWrite(Pins::drive_rgb_s3, LOW);
	redFreq = pulseIn(Pins::drive_rgb_sensorOut, LOW);
	redFreq = map(redFreq, 130, 555, 0, 255);
	if (redFreq < 50 && (redFreq < redFreq && redFreq < blueFreq)) {
		sensorRed = true;
		sensorBlue = false;
		sensorGreen = false;
		detectedColor = CanType::red;
	}
	AsyncHandler.addCallback(&checkColorSensorPhase2, 100);
}

void MotorDrive::checkColorSensorPhase2() {
	digitalWrite(Pins::drive_rgb_s2, HIGH);
	digitalWrite(Pins::drive_rgb_s3, HIGH);
	greenFreq = pulseIn(Pins::drive_rgb_sensorOut, LOW);
	greenFreq = map(greenFreq, 190, 1300, 0, 255);
	if (greenFreq < 50 && (greenFreq < redFreq && greenFreq < blueFreq)) {
		sensorGreen = true;
		sensorBlue = false;
		sensorRed = false;
		detectedColor = CanType::green;
	}
	AsyncHandler.addCallback(&checkColorSensorPhase3, 100);
}

void MotorDrive::checkColorSensorPhase3() {
	digitalWrite(Pins::drive_rgb_s2, LOW);
	digitalWrite(Pins::drive_rgb_s3, HIGH);
	blueFreq = pulseIn(Pins::drive_rgb_sensorOut, LOW);
	blueFreq = map(blueFreq, 55, 370, 0, 255);
	if (blueFreq < 50 && (blueFreq < greenFreq && blueFreq < redFreq)) {
		sensorBlue = true;
		sensorGreen = false;
		sensorRed = false;
		detectedColor = CanType::blue;
	}
	has_read_a_color = true;
	color_reading_in_progress = false;
	disable_color_sensor_until = millis() + 100;
	AsyncHandler.addCallback(&checkSensedColor);
}

void MotorDrive::checkSensedColor() {
	if (CanIntake::needsMoreCans(detectedColor)) {
		servoLeft.write(90);
		servoRight.write(90);
		updatePIDValues();
		AsyncHandler.removeCallback(&updatePIDValues);
		CanIntake::beginCollection(detectedColor);
	}
	else {
		AsyncHandler.addCallback(&checkColorSensor);
	}
}

//implements line following PID and adds values to drive
void MotorDrive::addLinePidValues(int dir) {
	static constexpr double kP = 0.02, kI = 0.0, kD = 0.0;
	static constexpr int leftBaseSpeed = 140, rightBaseSpeed = 140;
	static int currentPosition;
	static double previousError = 0;
	static int mapLower = 90, mapUpper = 90;

	int targetPosition = (blackTape + whiteTape) / 2;
	if (dir == 1) { //checks which sensor to read and prepares the mapping values accordingly
		currentPosition = analogRead(Pins::drive_forward_sensor);
		targetPosition = (blackTapeFwd + whiteTapeFwd)/2;
	}
	else {
		currentPosition = analogRead(Pins::drive_reverse_sensor);
		targetPosition = (blackTapeRwd + whiteTapeRwd)/2;
	}
	int error = targetPosition - currentPosition;

	double integral = 0;
	if (abs(error) < iZone) {
		integral = (error * kI) + integral;
	}else{
		integral = 0
	}

	double derivative = (error - previousError) * kD;
	previousError = error;

	outputSpeed = error * kP + integral + derivative;

	// Serial.print(currentPosition);
	// Serial.print("\t");
	// Serial.print(error);
	// Serial.print("\t");
	// Serial.print(outputSpeed);
	// Serial.print("\t");
	// Serial.print(constrain(rightMotorSpeed, 0, 180));
	// Serial.print("\t");
	// Serial.print(constrain(leftMotorSpeed, 0, 180));
	// Serial.print("\n");

	if (dir == 1){ //moving forwards
		leftBaseSpeed = 75;
		rightBaseSpeed = 105;
		int leftWrite = constrain(leftBaseSpeed - outputSpeed, 0, 80);
		int rightWrite = constrain(rightBaseSpeed - outputSpeed, 100, 180);
	}else{
		leftBaseSpeed = 112;
		rightBaseSpeed = 75;
		int leftWrite = constrain(leftBaseSpeed + outputSpeed, 100, 180);
		int rightWrite = constrain(rightBaseSpeed + outputSpeed, 0, 80);
 	}

	Serial.print(leftWrite);
	Serial.print("\t");
	Serial.print(rightWrite);
	servoLeft.write(leftWrite);
	servoRight.write(rightWrite);
	Serial.print("\n");
}

void MotorDrive::requestColor(CanType canColor) {
	requiredColor = canColor;
}
