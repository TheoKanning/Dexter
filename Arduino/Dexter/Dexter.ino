#include <SerialCommand.h>
#include <PID_v1.h>

#define BLUETOOTH_TIMEOUT 1000
#define FREQUENCY 200 // number of motor updates per second

const double Kp = 30;
const double Ki = 0;
const double Kd = 0.2;
const int fallThreshold = 15; // give up if robot is more than this many degrees from vertical

double setPoint = 0;
double pitch = 0;
double stepsPerSecond;

PID anglePid(&pitch, &stepsPerSecond, &setPoint, Kp, Ki, Kd, DIRECT);

long lastUpdateTime;
long lastPrintTimeMs;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(57600);
  delay(1000); // wait for serial to be available
  Serial.println("Dexter is starting...");
  digitalWrite(13, LOW);
  MPU6050_setup();
  calibrateImu();
  anglePid.SetMode(AUTOMATIC);
  anglePid.SetOutputLimits(-getMaxSpeed(), getMaxSpeed());
  enableSteppers();
  digitalWrite(13, HIGH);
}

void loop() {

  if (micros() - lastUpdateTime < 1000000 / FREQUENCY) {
    return;
  }
  lastUpdateTime = micros();
  pitch = updatePitch();
  
  if (fallen()) {
    stepsPerSecond = 1;
  } else {
    anglePid.Compute(); // updates stepsPerSecond
    // add dead band for low speeds
    //if (stepsPerSecond < 5 && stepsPerSecond > -5) stepsPerSecond = 0.1;
  }

  setLeftSpeed(stepsPerSecond);
  setRightSpeed(stepsPerSecond);

  //log();
}

void releaseMotors(){
  setLeftSpeed(0);
  setRightSpeed(0);
}

bool fallen() {
  return pitch > fallThreshold || pitch < - fallThreshold;
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

void log() {
  // log at a slower frequency
  const int logFrequency = 5;
  if (millis() - lastPrintTimeMs > 1000 / logFrequency) {
    lastPrintTimeMs = millis();
    
    Serial.print("Pitch: ");
    Serial.print(pitch);
    Serial.print(" StepsPerSecond:");
    Serial.println(stepsPerSecond);
  }
}
