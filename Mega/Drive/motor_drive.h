#pragma once
#ifndef MOTOR_DRIVE_H
#define MOTOR_DRIVE_H
#include <Arduino.h>
#include <Servo.h>
#include <Chrono.h>
//#include <Metro.h> // Include the Metro library
#include <AutoPID.h>
#include  "../MockAsync/async_handler.h"
#include "../shared_types.h"

static struct MotorDrive {
	static constexpr auto OUTPUT_PIN = A1;

	//main PID consts
	static constexpr auto OUTPUT_MIN_IR = 0;
	static constexpr auto OUTPUT_MAX_IR = 180;
	static constexpr auto KP_IR = 0.15;
	static constexpr auto KI_IR = 0.0003;
	static constexpr auto KD_IR = 0;

	//left and right pid consts
	static constexpr auto RIGHT_KP = 0.10;
	static constexpr auto RIGHT_KI = 0.0;
	static constexpr auto RIGHT_KD = 0.0;
	static constexpr auto LEFT_KP = 0.10;
	static constexpr auto LEFT_KI = 0.0;
	static constexpr auto LEFT_KD = 0.0;

	//rgb sensor
	static constexpr auto S0 = 4;
	static constexpr auto S1 = 5;
	static constexpr auto S2 = 6;
	static constexpr auto S3 = 7;
	static constexpr auto sensorOut = 8;

	static double countLeft;
	static double countRight;

	//static Metro serialMetro;  // Instantiate an instance
	//static Metro sensorMetro;

	static Chrono chrono;

	//will come from sensor
	static CanType detectedColor;

	//will eventually come from state.
	static CanType requiredColor;

	//-1 is reverse, 1 is forward
	static int dir;

	static int targetPos;

	//IR PID
	static double irRightAddition, irLeftAddition, driveLeftPidOutput, driveRightPidOutput;
	static double inputIR, setPointIR, outputFromIR;

	//DRIVE PIDS
	static double driveSetPoint;
	static double outputRDrive;
	static double outputLDrive;


	//rgb stuff
	static int frequency, redFreq, greenFreq, blueFreq;
	static bool sensorRed, sensorGreen, sensorBlue;

	// servos
	static Servo servoLeft;
	static Servo servoRight;

	static int throttleLeft, throttleRight;

	//pid loops
	static AutoPID sensorPID;// = AutoPID(&inputIR, &setPointIR, &outputFromIR, OUTPUT_MIN_IR, OUTPUT_MAX_IR, KP_IR, KI_IR, KD_IR);
	static AutoPID leftPID;// = AutoPID(&countLeft, &driveSetPoint, &outputLDrive, OUTPUT_MIN_IR, OUTPUT_MAX_IR, LEFT_KP, LEFT_KI, LEFT_KD);
	static AutoPID rightPID;// = AutoPID(&countRight, &driveSetPoint, &outputRDrive, OUTPUT_MIN_IR, OUTPUT_MAX_IR, RIGHT_KP, RIGHT_KI, RIGHT_KD);

	MotorDrive();

	static void ISRleft();
	static void ISRright();

	static bool inRange(int val, int minimum, int maximum);

	static void addLeftPidValues();
	static void addRightPidValues();

	//implements line following PID and adds values to drive
	static void addLinePidValues(int dir);

	static void requestColor(CanType canColor);

	static void checkColorSensor();

	static void updatePIDValues();

	static void driveSomewhere();

	//static void loop() {

	//	if (true) { // put your main code here, to run repeatedly:
	//		if (true) { //moveCommandFromStateIsTrue

	//		  //will come from state, 1,2,3 r,g,b
	//			int colourRequest = colour;
	//			if (colour == 0 && (countLeft > 0 || countRight > 0)) { // go back because we have everything
	//				dir = -1;
	//			}
	//			else {
	//				dir = 1;
	//			}

	//			checkColorSensor();

	//			if (colourRequest == detectedColour) {
	//				servoLeft.write(90);
	//				servoRight.write(90);
	//				return;
	//			} 

	//			//every 100 millis
	//			if (sensorMetro.check() == 1) {
	//				Serial.print("every100millis");
	//				addLeftPidValues();
	//				addRightPidValues();
	//				addLinePidValues(dir);
	//				servoLeft.write(constrain(throttleLeft, 0, 180) * dir);
	//				servoRight.write(constrain(throttleRight, 0, 180) * dir);
	//			}



	//		}

	//	}
	//	else { //power off stop robot
	//		servoLeft.write(90);
	//		servoRight.write(90);
	//	}
	//}

private:
	static bool has_read_a_color;
	static bool color_reading_in_progress;
	static Chrono::chrono_t disable_color_sensor_until;

	static void checkColorSensorPhase1();
	static void checkColorSensorPhase2();
	static void checkColorSensorPhase3();
	static void checkSensedColor();
} MotorDrive;

#endif