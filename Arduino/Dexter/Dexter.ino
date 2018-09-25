#include <SoftwareSerial.h>
#include <Adafruit_MotorShield.h>
#include <SerialCommand.h>

#define FRONT_LEFT_MOTOR_NUMBER   1
#define BACK_LEFT_MOTOR_NUMBER    2
#define BACK_RIGHT_MOTOR_NUMBER   3
#define FRONT_RIGHT_MOTOR_NUMBER  4

#define PWM_MIN -255
#define PWM_MAX  255
#define MIN_SPEED 50
#define BLUETOOTH_TIMEOUT 1000

SoftwareSerial btSerial(3, 2); //RX | TX pins
SoftwareSerial imuSerial(5, 4);

SerialCommand serialCommand(imuSerial);

Adafruit_MotorShield motorManager;
Adafruit_DCMotor *motorFrontLeft;
Adafruit_DCMotor *motorBackLeft;
Adafruit_DCMotor *motorBackRight;
Adafruit_DCMotor *motorFrontRight;

int rightSideMotorSpeedCommand;
int leftSideMotorSpeedCommand;
long lastUpdateTimeMs = millis();


void setup() {
  Serial.begin(57600);
  Serial.println("Starting setup");
  //btSerial.begin(38400); //Baud rate may vary depending on your chip's settings!
  imuSerial.begin(9600);
  imuSerial.listen();
  serialCommand.addCommand("R", rollReceived);
  serialCommand.addCommand("P", pitchReceived);
  serialCommand.addCommand("Y", yawReceived);
  serialCommand.addDefaultHandler(unrecognized);
  initMotors();
  
}
/*
* Loads serial data if available, releases motors after 1 second with data
*/
void loop() {
  serialCommand.readSerial();
//  if(readBtSerialData()){
//    lastUpdateTimeMs = millis();
//    setMotorSpeeds(rightSideMotorSpeedCommand, leftSideMotorSpeedCommand);
//  } else if(millis() - lastUpdateTimeMs > BLUETOOTH_TIMEOUT) {
//    Serial.println("Bluetooth timed out, releasing motors");
//    lastUpdateTimeMs = millis();
//    releaseMotors();
//  }
}

void rollReceived() {
  char *roll = serialCommand.next();
  Serial.print("Roll: ");
  Serial.println(roll);
}

void pitchReceived() {
  char *pitch = serialCommand.next();
  Serial.print("Pitch: ");
  Serial.println(pitch);
}

void yawReceived() {
  char *yaw = serialCommand.next();
  Serial.print("Yaw: ");
  Serial.println(yaw);
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

void initMotors(){
  motorManager = Adafruit_MotorShield();
  motorFrontLeft   = motorManager.getMotor(FRONT_LEFT_MOTOR_NUMBER);
  motorBackLeft    = motorManager.getMotor(BACK_LEFT_MOTOR_NUMBER);
  motorBackRight   = motorManager.getMotor(BACK_RIGHT_MOTOR_NUMBER);
  motorFrontRight  = motorManager.getMotor(FRONT_RIGHT_MOTOR_NUMBER);
  
  motorManager.begin();
  releaseMotors();
}

/*
* Sets all motors speeds, assumes good data
*/
void setMotorSpeeds(int rightMotorSpeed, int leftMotorSpeed){
  setSingleMotorSpeed(motorFrontRight, rightMotorSpeed);
  setSingleMotorSpeed(motorBackRight, rightMotorSpeed);
  setSingleMotorSpeed(motorFrontLeft, leftMotorSpeed);
  setSingleMotorSpeed(motorBackLeft, leftMotorSpeed);
}

/*
* Sets speed of a single motor, assumes good data
*/
void setSingleMotorSpeed(Adafruit_DCMotor *motor, int motorSpeed){
  if(abs(motorSpeed) < MIN_SPEED){
    motor->run(RELEASE);
  } else if (motorSpeed > 0) {
    motor->setSpeed(motorSpeed);
    motor->run(FORWARD);
  } else {
    motor->setSpeed(abs(motorSpeed));
    motor->run(BACKWARD);
  }
}

void releaseMotors(){
  motorFrontLeft->run(RELEASE);
  motorBackLeft->run(RELEASE);
  motorBackRight->run(RELEASE);
  motorFrontRight->run(RELEASE);
}

void echoImuSerial() {
  if (imuSerial.available() == 0) {
    return;
  }

  while (imuSerial.available() > 0) {
    char inByte = imuSerial.read();
    Serial.write(inByte);
  }
  Serial.println();
}

/* 
* Reads available bluetooth serial data and returns true if a complete set has been read
* Updates rightSideMotorSpeedCommand and leftSideMotorSpeedCommand global variables
*/
boolean readBtSerialData(){
  const int leftBit = 1;
  const int rightBit = 2;
  const int doneBit = 3;
  
  /* Temporary variable for storing progress, 
  * done == doneBit once right and left each been read at least once
  * Uses OR instead of + to prevent errors if one side is read multiple times
  * Ex. done = 0: done |= rightBit; done |= rightBit; done |= rightBit; done == 1
  * done = 0; done += rightBit; done += rightBit; done += rightBit; done == 3; <-- Considered complete without reading ledt side
  */
  int done = 0; 
  char buffer = '\0';
  
  while(btSerial.available() > 0 && done != doneBit){
    buffer = btSerial.read();
    switch(buffer){
      case 'R':
        rightSideMotorSpeedCommand = constrain(btSerial.parseFloat(), PWM_MIN, PWM_MAX);
        done |= rightBit;
        break;
      case 'L':
        leftSideMotorSpeedCommand = constrain(btSerial.parseFloat(), PWM_MIN, PWM_MAX);
        done |= leftBit;
        break;
    }
  } 
  if(done == doneBit) {
    btSerial.flush();
    Serial.print("Right:"); Serial.println(rightSideMotorSpeedCommand); 
    Serial.print(" Left:"); Serial.println(leftSideMotorSpeedCommand);
    return true;
  } else {
    rightSideMotorSpeedCommand = 0;
    leftSideMotorSpeedCommand = 0; 
    return false;
  }
}
