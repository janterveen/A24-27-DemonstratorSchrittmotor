/**@file */

/**
 * @brief Programm für den Test der Funktionalität eines Stepper-Motors
 Beschreibung des Autors:
 Example sketch to control a stepper motor with A4988 stepper motor driver, 
  AccelStepper library and Arduino: continuous rotation. 
  More info: https://www.makerguides.com 
 */


// Include the AccelStepper library:
#include "AccelStepper.h"

// Define stepper motor connections and motor interface type. 
// Motor interface type must be set to 1 when using a driver
#define dirPin 0
#define stepPin 8
#define motorInterfaceType 1

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
  // Set the maximum speed in steps per second:
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(1000);
}

void loop() {
  // Set the speed in steps per second:
  stepper.setSpeed(500);
  // Step the motor with a constant speed as set by setSpeed():
  stepper.runSpeed();
}