#pragma once
#ifndef MOTOR_DRIVE_H
#define MOTOR_DRIVE_H
#include <Arduino.h>
#include <Servo.h>
#include <Chrono.h>
//#include <Metro.h> // Include the Metro library
#include <AutoPID.h>
#include  "async_handler.h"
#include "shared_types.h"

struct MotorDrive {

	static void init();
	static void driveSomewhere();
	static bool isDriving() noexcept;
	static bool tripInProgress() noexcept;

private:
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
//static constexpr auto S0 = 4;
//static constexpr auto S1 = 5;
//static constexpr auto S2 = 6;
//static constexpr auto S3 = 7;
//static constexpr auto sensorOut = 8;

	static double countLeft;
	static double countRight;

	//static Metro serialMetro;  // Instantiate an instance
	//static Metro sensorMetro;


	//-1 is reverse, 1 is forward
	static int dir;

	static int targetDistance;

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

	static bool is_driving;
	static bool trip_in_progress;

	// servos
	static Servo servoLeft;
	static Servo servoRight;

	static int throttleLeft, throttleRight;

	//pid loops
	static AutoPID sensorPID;// = AutoPID(&inputIR, &setPointIR, &outputFromIR, OUTPUT_MIN_IR, OUTPUT_MAX_IR, KP_IR, KI_IR, KD_IR);
	static AutoPID leftPID;// = AutoPID(&countLeft, &driveSetPoint, &outputLDrive, OUTPUT_MIN_IR, OUTPUT_MAX_IR, LEFT_KP, LEFT_KI, LEFT_KD);
	static AutoPID rightPID;// = AutoPID(&countRight, &driveSetPoint, &outputRDrive, OUTPUT_MIN_IR, OUTPUT_MAX_IR, RIGHT_KP, RIGHT_KI, RIGHT_KD);

	static bool color_reading_in_progress;
	static millis_t disable_color_sensor_until;

	static void checkColorSensorPhase1();
	static void checkColorSensorPhase2();
	static void checkColorSensorPhase3();
	static void checkSensedColor();

	static void stopMovement();

	static void ISRleft();
	static void ISRright();

	static bool inRange(int val, int minimum, int maximum);

	//implements line following PID and adds values to drive
	static void addLinePidValues();

	static void checkColorSensor();

	static void driveLoop();

};

#endif