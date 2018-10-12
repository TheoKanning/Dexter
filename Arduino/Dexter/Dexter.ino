#include <SoftwareSerial.h>
#include <SerialCommand.h>
#include "Controller.h"
#include <PID_v1.h>

#define BLUETOOTH_TIMEOUT 1000
#define FREQUENCY 20 // number of motor updates per second

SoftwareSerial btSerial(3, 2); //RX | TX pins
SoftwareSerial imuSerial(5, 4);

SerialCommand serialCommand(imuSerial);

Controller controller;

long lastUpdateTimeMs = millis();
int fallThreshold = 15; // give up if robot is more than this many degrees from vertical
boolean fallen = false;
float pitchCorrection = -0.87; // The pitch value when vertical, corrects construction errors

void setup() {
  Serial.begin(57600);
  Serial.println("Dexter is starting...");
  //btSerial.begin(38400); //Baud rate may vary depending on your chip's settings!
  imuSerial.begin(38400);
  imuSerial.listen();
  serialCommand.addCommand("R", rollReceived);
  serialCommand.addCommand("P", pitchReceived);
  serialCommand.addCommand("Y", yawReceived);
  serialCommand.addDefaultHandler(unrecognized);
  enableSteppers();
}

void loop() {
  checkForFall(controller.pitch);
  
  if (fallen) {
    releaseMotors();
  } 
  
  if (millis() - lastUpdateTimeMs < 1000 / FREQUENCY) {
    return;
  }
  lastUpdateTimeMs = millis();
  
  serialCommand.readSerial();
  
  MotorSpeed speeds = controller.calculateMotorSpeeds(0);
  setLeftSpeed(speeds.left);
  setRightSpeed(speeds.right);
}

void rollReceived() {
  double roll = wrapAngle(atof(serialCommand.next()));
  //logAngle("Roll", roll);
  controller.roll = roll;
}

void pitchReceived() {
  double pitch = wrapAngle(atof(serialCommand.next())) - pitchCorrection;
  //logAngle("Pitch", pitch);
  controller.pitch = 0.5 * controller.pitch + 0.5 * pitch;
}

void yawReceived() {
  double yaw = wrapAngle(atof(serialCommand.next()));
  //logAngle("Yaw", yaw);
  controller.yaw = yaw;
}

void unrecognized() {
  char *next = serialCommand.next();
  Serial.print("Unrecognized command: ");
  if (next != NULL) {
    Serial.println(next);
  } else {
    Serial.println("No second argument");
  }
}

void releaseMotors(){
  setLeftSpeed(0);
  setRightSpeed(0);
}

void checkForFall(double pitch) {
  fallen = pitch > fallThreshold || pitch < - fallThreshold;
}

// adds and subtracts 360 degrees until angle is between -180 and 180
double wrapAngle(double angle) {
  while(angle > 180) {
    angle -= 360;
  }
  while(angle < -180) {
    angle += 360;
  }
  return angle;
}

void logAngle(char* name, double angle){
  Serial.print(name);
  Serial.print(": ");
  Serial.println(angle);
}
