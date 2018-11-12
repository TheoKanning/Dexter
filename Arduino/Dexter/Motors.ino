#include <TimerOne.h>
#include <TimerThree.h>

#define TIMER_FREQ 1000000

const int microsteps = 8; // A4988 is set for 1/8th steps

const int leftStepPin  = 23;
const int leftDirPin   = 22;
const int rightStepPin = 21;
const int rightDirPin  = 20;

void leftStep() {
  digitalWrite(leftStepPin, HIGH);
  digitalWrite(leftStepPin, LOW);
}

void rightStep() {
  digitalWrite(rightStepPin, HIGH);
  digitalWrite(rightStepPin, LOW);
}

void enableSteppers() {
  pinMode(rightStepPin, OUTPUT);
  pinMode(rightDirPin, OUTPUT);
  pinMode(leftStepPin, OUTPUT);
  pinMode(leftDirPin, OUTPUT);
  Timer1.initialize(TIMER_FREQ);
  Timer1.attachInterrupt(leftStep);
  Timer3.initialize(TIMER_FREQ);
  Timer3.attachInterrupt(rightStep);
}

void setLeftSpeed(double stepsPerSecond) {
  stepsPerSecond = constrain(stepsPerSecond, -MAX_SPEED, MAX_SPEED);
  Timer1.setPeriod(TIMER_FREQ / abs(stepsPerSecond * microsteps));
  setDirection(leftDirPin, -stepsPerSecond); // negative for left motor
}

void setRightSpeed(double stepsPerSecond) {
  stepsPerSecond = constrain(stepsPerSecond, -MAX_SPEED, MAX_SPEED);
  Timer3.setPeriod(TIMER_FREQ / abs(stepsPerSecond * microsteps));
  setDirection(rightDirPin, stepsPerSecond);
}

void setDirection(int pin, int stepsPerSecond) {
  if (stepsPerSecond > 0) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}
