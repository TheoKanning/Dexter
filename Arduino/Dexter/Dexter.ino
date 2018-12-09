#define BLUETOOTH_TIMEOUT 1000
#define FREQUENCY 100 // number of motor updates per second

#define MAX_SPEED 500
#define MAX_ANGLE 15

#define LOG_IMU false
#define LOG_SPEED_PID false
#define LOG_ANGLE_PID true
#define LOG Serial //Serial or Serial1
#define TUNE_SERIAL Serial1

double speedKp = 0.012;
double speedKi = 0.0009;
double angleKp = 95;
double angleKd = 0.45;
const int fallThreshold = MAX_ANGLE; // give up if robot is more than this many degrees from vertical

double setSpeed = 0;
double setAngle = 0;
double speed = 0;
double stepsPerSecond;

long lastUpdateTime;
long lastSteerTime;
long lastPrintTimeMs;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(57600);
  Serial1.begin(38400);
  MPU6050_setup();
  delay(1000); // pause before starting IMU calibration
  digitalWrite(13, LOW); // turn off LED while calibrating
  calibrateImu();
  enableSteppers();
  digitalWrite(13, HIGH);
}

void loop() {

  if (micros() - lastUpdateTime < 1000000 / FREQUENCY) {
    return;
  }

  checkForPidCommands();
  
  lastUpdateTime = micros();
  float pitch = updatePitch();
  
  if (fallen(pitch)) {
    stepsPerSecond = 1;
  } else {
    speed = 0.9 * speed + 0.1 * stepsPerSecond;
    setAngle = speedPid(speed, setSpeed);
    stepsPerSecond = anglePid(pitch, setAngle);
    stepsPerSecond = 0.2;
  }

  setLeftSpeed(stepsPerSecond);
  setRightSpeed(stepsPerSecond);
}

bool fallen(float pitch) {
  return pitch > fallThreshold || pitch < - fallThreshold;
}

void checkForPidCommands() {
  if (TUNE_SERIAL.available()) {
    char key = (char)TUNE_SERIAL.read();
    bool changed = true;
    switch(key) {
       case 'f':
        setSpeed = 10;
        lastSteerTime = millis();
        break;
      case 'b':
        setSpeed = -10;
        lastSteerTime = millis();
        break;
      case 'P':
        angleKp += 1;
        break;
      case 'p':
        angleKp -= 1;
        break;
      case 'D':
        angleKd += 0.025;
        break;
      case 'd':
        angleKd -= 0.025;
        break;
      case 'S':
        speedKp += 0.001;
        break;
      case 's':
        speedKp -= 0.001;
        break;
      case 'I':
        speedKi += 0.00015;
        break;
      case 'i':
        speedKi -= 0.00015;
        break;
      default:
        changed = false;
        break;
    }
    if (changed) {
      TUNE_SERIAL.print("angleKp: ");
      TUNE_SERIAL.print(angleKp);
      TUNE_SERIAL.print(" angleKd: ");
      TUNE_SERIAL.print(angleKd);
      TUNE_SERIAL.print(" speedKp: ");
      TUNE_SERIAL.print(speedKp, 4);
      TUNE_SERIAL.print(" speedKi: ");
      TUNE_SERIAL.print(speedKi, 4);
      TUNE_SERIAL.print(" Steps: ");
      TUNE_SERIAL.println(stepsPerSecond);
    }
  }
}
