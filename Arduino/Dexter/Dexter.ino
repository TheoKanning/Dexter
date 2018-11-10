#define BLUETOOTH_TIMEOUT 1000
#define FREQUENCY 100 // number of motor updates per second

#define MAX_SPEED 500
#define MAX_ANGLE 15

#define LOG_IMU false
#define LOG_SPEED_PID false
#define LOG_ANGLE_PID true
#define LOG Serial //Serial or Serial1

double speedKp = 0;
double speedKd = 0;
double angleKp = 50;
double angleKd = 1;
const int fallThreshold = MAX_ANGLE; // give up if robot is more than this many degrees from vertical

double setSpeed = 0;
double setAngle = 0;
double pitch = 0;
double stepsPerSecond;

long lastUpdateTime;
long lastSteerTime;
long lastPrintTimeMs;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(57600);
  Serial1.begin(38400);
  Serial.println("Dexter is starting...");
  Serial1.println("Dexter is starting...");
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
    setAngle = speedPid(stepsPerSecond, setSpeed);
    stepsPerSecond = anglePid(pitch, setAngle);
  }

  setLeftSpeed(stepsPerSecond);
  setRightSpeed(stepsPerSecond);
}

bool fallen() {
  return pitch > fallThreshold || pitch < - fallThreshold;
}

void checkForPidCommands() {
  if (Serial1.available()) {
    char key = (char)Serial1.read();
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
      default:
        changed = false;
        break;
    }
    if (changed) {
      LOG.print("angleKp: ");
      LOG.print(angleKp);
      LOG.print(" angleKd: ");
      LOG.print(angleKd);
//      Serial1.print("speedKp: ");
//      Serial1.print(speedKp, 4);
//      Serial1.print(" speedKd: ");
//      Serial1.print(speedKd, 4);
      LOG.print(" Pitch: ");
      LOG.print(pitch);
      LOG.print(" Steps: ");
      LOG.println(stepsPerSecond);
    }
  }
}
