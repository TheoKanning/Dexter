#include <SoftwareSerial.h>
#include <SerialCommand.h>
#include <PID_v1.h>

#define BLUETOOTH_TIMEOUT 1000
#define FREQUENCY 20 // number of motor updates per second

const double Kp = 0;
const double Ki = 0;
const double Kd = 0;
const int fallThreshold = 15; // give up if robot is more than this many degrees from vertical
const float pitchCorrection = -0.87; // The pitch value when vertical, corrects construction errors

SoftwareSerial btSerial(3, 2); //RX | TX pins
SoftwareSerial imuSerial(5, 4);

SerialCommand serialCommand(imuSerial);

double setPoint = 0;
double pitch = 0;
double stepsPerSecond;

PID anglePid(&pitch, &stepsPerSecond, &setPoint, Kp, Ki, Kd, DIRECT);

long lastUpdateTimeMs = millis();
void setup() {
  Serial.begin(57600);
  Serial.println("Dexter is starting...");
  //btSerial.begin(38400); //Baud rate may vary depending on your chip's settings!
  //imuSerial.begin(38400);
  //imuSerial.listen();
  addSerialHandlers();
  anglePid.SetMode(AUTOMATIC);
  anglePid.SetOutputLimits(-getMaxSpeed(), getMaxSpeed());
  enableSteppers();
}

void loop() {
  
  if (millis() - lastUpdateTimeMs < 1000 / FREQUENCY) {
    return;
  }
  lastUpdateTimeMs = millis();
  
  serialCommand.readSerial();
  
  if (fallen()) {
    stepsPerSecond = 0;
  } else {
    anglePid.Compute(); // updates stepsPerSecond
  }
  
  Serial.println(stepsPerSecond);
  setLeftSpeed(stepsPerSecond);
  setRightSpeed(stepsPerSecond);
}

void addSerialHandlers() {
  serialCommand.addCommand("R", rollReceived);
  serialCommand.addCommand("P", pitchReceived);
  serialCommand.addCommand("Y", yawReceived);
  serialCommand.addDefaultHandler(unrecognized);
}

void rollReceived() {}
void yawReceived() {}
void pitchReceived() {
  double newPitch = wrapAngle(atof(serialCommand.next())) - pitchCorrection;
  //logAngle("Pitch", pitch);
  pitch = 0.5 * pitch + 0.5 * newPitch;
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

bool fallen() {
  pitch > fallThreshold || pitch < - fallThreshold;
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
