#define BLUETOOTH_TIMEOUT 1000
#define FREQUENCY 100 // number of motor updates per second

#define MAX_SPEED 500
#define MAX_ANGLE 15

#define LOG_IMU false
#define LOG_SPEED_PID false
#define LOG_ANGLE_PID true
#define LOG Serial //Serial or Serial1

double speedKp = 0;
double speedKi = 0;
double angleKp = 50;
double angleKd = 0.45;
const int fallThreshold = MAX_ANGLE; // give up if robot is more than this many degrees from vertical

double setSpeed = 0;
double setAngle = 0;
double pitch = 0;
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
  pitch = updatePitch();
  
  if (fallen()) {
    stepsPerSecond = 1;
  } else {
    speed = 0.9 * speed + 0.1 * stepsPerSecond;
    setAngle = speedPid(speed, setSpeed);
    stepsPerSecond = anglePid(pitch, setAngle);
  }

  setLeftSpeed(stepsPerSecond);
  setRightSpeed(stepsPerSecond);
}

bool fallen() {
  return pitch > fallThreshold || pitch < - fallThreshold;
}

void checkForPidCommands() {
  if (Serial.available()) {
    char key = (char)Serial.read();
    bool changed = true;
    switch(key) {
       case 'f':
        setAngle = -1;
        lastSteerTime = millis();
        break;
      case 'b':
        setAngle = 1;
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
        speedKi += 0.0005;
        break;
      case 'i':
        speedKi -= 0.00015;
        break;
      default:
        changed = false;
        break;
    }
    if (changed) {
      LOG.print("angleKp: ");
      LOG.print(angleKp);
      LOG.print(" angleKd: ");
      LOG.print(angleKd);
      LOG.print(" speedKp: ");
      LOG.print(speedKp, 4);
      LOG.print(" speedKi: ");
      LOG.print(speedKi, 4);
      LOG.print(" Pitch: ");
      LOG.print(pitch);
      LOG.print(" Steps: ");
      LOG.println(stepsPerSecond);
    }
  }
}
