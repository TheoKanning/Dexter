#define FREQUENCY 100 // number of motor updates per second
#define BLUETOOTH_TIMEOUT 1000 // stop after this many milliseconds without a bluetooth command

#define MAX_SPEED 500
#define MAX_ANGLE 15

#define BLUETOOTH Serial1
#define USB Serial
#define LOG_IMU false
#define LOG_SPEED_PID false
#define LOG_ANGLE_PID false
#define LOG USB //BLUETOOTH or USB
#define TUNE_SERIAL BLUETOOTH

double speedKp = 0.027;
double speedKi = 0.0006;
double angleKp = 50;
double angleKd = 0.45;
const int fallThreshold = MAX_ANGLE; // give up if robot is more than this many degrees from vertical

double setSpeed = 0;
double setAngle = 0;
double maxAccel = 1;
double speed = 0;
double stepsPerSecond;
double differential = 0;

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

  if (lastSteerTime < millis() - BLUETOOTH_TIMEOUT) {
    setSpeed = 0;
    differential = 0;
  }

  // setSpeed = twiddle(speed, pitch);
  
  if (fallen(pitch)) {
    stepsPerSecond = 1;
  } else {
    speed = 0.9 * speed + 0.1 * stepsPerSecond;
    setAngle = speedPid(speed, setSpeed);
    stepsPerSecond = anglePid(pitch, setAngle);
  }

  setLeftSpeed(stepsPerSecond - differential);
  setRightSpeed(stepsPerSecond + differential);
}

bool fallen(float pitch) {
  return pitch > fallThreshold || pitch < - fallThreshold;
}

void checkForPidCommands() {
  if (TUNE_SERIAL.available()) {
    char key = (char)TUNE_SERIAL.read();
    USB.print(key);
    bool changed = true;
    switch(key) {
       case 'f':
        setSpeed = 50;
        lastSteerTime = millis();
        break;
      case 'b':
        setSpeed = -30;
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
        speedKi += 0.0001;
        break;
      case 'i':
        speedKi -= 0.0001;
        break;
      case 't':
        startTwiddling();
        break;
      case 'L':
        setSpeed = TUNE_SERIAL.parseFloat() * 100;
        lastSteerTime = millis();
        break;
      case 'A':
        differential = TUNE_SERIAL.parseFloat() * 30;
        lastSteerTime = millis();
      default:
        changed = false;
        break;
    }
    if (changed) {
      lastSteerTime = millis();
      TUNE_SERIAL.print("\n\n\nspeedKp: ");
      TUNE_SERIAL.println(speedKp, 3);
      TUNE_SERIAL.print(" speedKi: ");
      TUNE_SERIAL.println(speedKi, 4);
      TUNE_SERIAL.print(" angleKp: ");
      TUNE_SERIAL.println(angleKp);
      TUNE_SERIAL.print(" angleKd: ");
      TUNE_SERIAL.println(angleKd, 3);
      TUNE_SERIAL.print(" Steps: ");
      TUNE_SERIAL.println(stepsPerSecond);
    }
  }
}
