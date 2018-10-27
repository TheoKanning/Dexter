/* MPU9250 Basic Example Code
 by: Kris Winer
 date: April 1, 2014
 license: Beerware - Use this code however you'd like. If you
 find it useful you can buy me a beer some time.
 Modified by Brent Wilkins July 19, 2016

 Demonstrate basic MPU-9250 functionality including parameterizing the register
 addresses, initializing the sensor, getting properly scaled accelerometer,
 gyroscope, and magnetometer data out. Added display functions to allow display
 to on breadboard monitor. Addition of 9 DoF sensor fusion using open source
 Madgwick and Mahony filter algorithms. Sketch runs on the 3.3 V 8 MHz Pro Mini
 and the Teensy 3.1.

 SDA and SCL should have external pull-up resistors (to 3.3V).
 10k resistors are on the EMSENSR-9250 breakout board.

 Hardware setup:
 MPU9250 Breakout --------- Arduino
 VDD ---------------------- 3.3V
 SDA ----------------------- A4
 SCL ----------------------- A5
 GND ---------------------- GND
 */

#include "quaternionFilters.h"
#include "MPU9250.h"

#define SerialDebug false  // Set to true to get Serial output for debugging

float accel_bias[3];
float gyro_bias[3];

MPU9250 myIMU;

float testPitch = 0;
int testPitchCount = 0;

void enableImu()
{
  Wire.begin();

  // Read the WHO_AM_I register, this is a good test of communication
  byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  Serial.print("MPU9250 "); Serial.print("I AM "); Serial.print(c, HEX);
  Serial.print(" I should be "); Serial.println(0x71, HEX);

  if (c == 0x71) // WHO_AM_I should always be 0x68
  {
    Serial.println("MPU9250 is online...");

    // Start by performing self test and reporting values
    myIMU.MPU9250SelfTest(myIMU.SelfTest);
    Serial.print("x-axis self test: acceleration trim within : ");
    Serial.print(myIMU.SelfTest[0],1); Serial.println("% of factory value");
    Serial.print("y-axis self test: acceleration trim within : ");
    Serial.print(myIMU.SelfTest[1],1); Serial.println("% of factory value");
    Serial.print("z-axis self test: acceleration trim within : ");
    Serial.print(myIMU.SelfTest[2],1); Serial.println("% of factory value");
    Serial.print("x-axis self test: gyration trim within : ");
    Serial.print(myIMU.SelfTest[3],1); Serial.println("% of factory value");
    Serial.print("y-axis self test: gyration trim within : ");
    Serial.print(myIMU.SelfTest[4],1); Serial.println("% of factory value");
    Serial.print("z-axis self test: gyration trim within : ");
    Serial.print(myIMU.SelfTest[5],1); Serial.println("% of factory value");

    // Calibrate gyro and accelerometers, load biases in bias registers
    myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);
    Serial.print("Calculated gyro biases");
    Serial.print(myIMU.gyroBias[0]);
    Serial.print(" ");
    Serial.print(myIMU.gyroBias[1]);
    Serial.print(" ");
    Serial.println(myIMU.gyroBias[2]);
    
    Serial.print("Calculated accel biases");
    Serial.print(myIMU.accelBias[0]);
    Serial.print(" ");
    Serial.print(myIMU.accelBias[1]);
    Serial.print(" ");
    Serial.println(myIMU.accelBias[2]);

    myIMU.initMPU9250();
    // Initialize device for active mode read of acclerometer, gyroscope, and
    // temperature
    Serial.println("MPU9250 initialized for active data mode....");

    // Read the WHO_AM_I register of the magnetometer, this is a good test of
    // communication
    byte d = myIMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    Serial.print("AK8963 "); Serial.print("I AM "); Serial.print(d, HEX);
    Serial.print(" I should be "); Serial.println(0x48, HEX);

    // Get magnetometer calibration from AK8963 ROM
    myIMU.initAK8963(myIMU.magCalibration);
    // Initialize device for active mode read of magnetometer
    Serial.println("AK8963 initialized for active data mode....");
    if (SerialDebug)
    {
      //  Serial.println("Calibration values: ");
      Serial.print("X-Axis sensitivity adjustment value ");
      Serial.println(myIMU.magCalibration[0], 2);
      Serial.print("Y-Axis sensitivity adjustment value ");
      Serial.println(myIMU.magCalibration[1], 2);
      Serial.print("Z-Axis sensitivity adjustment value ");
      Serial.println(myIMU.magCalibration[2], 2);
    }
  } // if (c == 0x71)
  else
  {
    Serial.print("Could not connect to MPU9250: 0x");
    Serial.println(c, HEX);
    while(1) ; // Loop forever if communication doesn't happen
  }
}

bool imuUpdateAvailable() {
  // If intPin goes high, all data registers have new data
  // On interrupt, check if data ready interrupt
  return myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01;
}

void calibrateImu() {
  // read accel and gyro for 2 seconds and subtract the results from subsequent measurements

  long startTimeMs = millis();
  int count = 0;

  myIMU.getAres();
  myIMU.getGres();

  while(millis() - startTimeMs < 2000) {
    myIMU.readAccelData(myIMU.accelCount);
    accel_bias[0] += myIMU.accelCount[0]*myIMU.aRes;
    accel_bias[1] += myIMU.accelCount[1]*myIMU.aRes;
    accel_bias[2] += 1 - myIMU.accelCount[2]*myIMU.aRes;

    myIMU.readGyroData(myIMU.gyroCount);
    gyro_bias[0] += myIMU.gyroCount[0]*myIMU.gRes;
    gyro_bias[1] += myIMU.gyroCount[1]*myIMU.gRes;
    gyro_bias[2] += myIMU.gyroCount[2]*myIMU.gRes;
    
    delay(50);
    count++;
  }

    accel_bias[0] /= count;
    accel_bias[1] /= count;
    accel_bias[2] /= count;
    gyro_bias[0] /= count;
    gyro_bias[1] /= count;
    gyro_bias[2] /= count;

    Serial.print("Calculated accel biases ");
    Serial.print(accel_bias[0]);
    Serial.print(" ");
    Serial.print(accel_bias[1]);
    Serial.print(" ");
    Serial.println(accel_bias[2]);
    Serial.print("Calculated gyro biases ");
    Serial.print(gyro_bias[0]);
    Serial.print(" ");
    Serial.print(gyro_bias[1]);
    Serial.print(" ");
    Serial.println(gyro_bias[2]);
}

void updateImu() {

  if (!imuUpdateAvailable()) return;
  
  myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
  myIMU.getAres();

  // Now we'll calculate the accleration value into actual g's
  // This depends on scale being set
  myIMU.ax = (float)myIMU.accelCount[0]*myIMU.aRes; // - accelBias[0];
  myIMU.ay = (float)myIMU.accelCount[1]*myIMU.aRes; // - accelBias[1];
  myIMU.az = (float)myIMU.accelCount[2]*myIMU.aRes; // - accelBias[2];

  myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values
  myIMU.getGres();

  // Calculate the gyro value into actual degrees per second
  // This depends on scale being set
  myIMU.gx = (float)myIMU.gyroCount[0]*myIMU.gRes;
  myIMU.gy = (float)myIMU.gyroCount[1]*myIMU.gRes;
  myIMU.gz = (float)myIMU.gyroCount[2]*myIMU.gRes;

  myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
  myIMU.getMres();
  // User environmental x-axis correction in milliGauss, should be
  // automatically calculated
  myIMU.magbias[0] = +470.;
  // User environmental x-axis correction in milliGauss TODO axis??
  myIMU.magbias[1] = +120.;
  // User environmental x-axis correction in milliGauss
  myIMU.magbias[2] = +125.;

  // Calculate the magnetometer values in milliGauss
  // Include factory calibration per data sheet and user environmental
  // corrections
  // Get actual magnetometer value, this depends on scale being set
  myIMU.mx = (float)myIMU.magCount[0]*myIMU.mRes*myIMU.magCalibration[0] -
             myIMU.magbias[0];
  myIMU.my = (float)myIMU.magCount[1]*myIMU.mRes*myIMU.magCalibration[1] -
             myIMU.magbias[1];
  myIMU.mz = (float)myIMU.magCount[2]*myIMU.mRes*myIMU.magCalibration[2] -
             myIMU.magbias[2];

  // Must be called before updating quaternions!
  myIMU.updateTime();

  // Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
  // the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
  // (+ up) of accelerometer and gyro! We have to make some allowance for this
  // orientationmismatch in feeding the output to the quaternion filter. For the
  // MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
  // along the x-axis just like in the LSM9DS0 sensor. This rotation can be
  // modified to allow any convenient orientation convention. This is ok by
  // aircraft orientation standards! Pass gyro rate as rad/s
  //MadgwickQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, 
  //myIMU.gx*PI/180.0f, myIMU.gy*PI/180.0f, myIMU.gz*PI/180.0f,  
  //myIMU.mx,  myIMU.my, myIMU.mz);
  MadgwickQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx*DEG_TO_RAD,
                         myIMU.gy*DEG_TO_RAD, myIMU.gz*DEG_TO_RAD, myIMU.my,
                         myIMU.mx, myIMU.mz, myIMU.deltat);

// Define output variables from updated quaternion---these are Tait-Bryan
// angles, commonly used in aircraft orientation. In this coordinate system,
// the positive z-axis is down toward Earth. Yaw is the angle between Sensor
// x-axis and Earth magnetic North (or true North if corrected for local
// declination, looking down on the sensor positive yaw is counterclockwise.
// Pitch is angle between sensor x-axis and Earth ground plane, toward the
// Earth is positive, up toward the sky is negative. Roll is angle between
// sensor y-axis and Earth ground plane, y-axis up is positive roll. These
// arise from the definition of the homogeneous rotation matrix constructed
// from quaternions. Tait-Bryan angles as well as Euler angles are
// non-commutative; that is, the get the correct orientation the rotations
// must be applied in the correct order which for this configuration is yaw,
// pitch, and then roll.
// For more see
// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
// which has additional links.

      const float * q = getQ();

      myIMU.yaw   = atan2(2.0f * (*(getQ()+1) * *(getQ()+2) + *getQ() *
                    *(getQ()+3)), *getQ() * *getQ() + *(getQ()+1) * *(getQ()+1)
                    - *(getQ()+2) * *(getQ()+2) - *(getQ()+3) * *(getQ()+3));
      
      myIMU.pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
      myIMU.pitch *= RAD_TO_DEG;
      myIMU.roll  = atan2(2.0f * (*getQ() * *(getQ()+1) + *(getQ()+2) *
                    *(getQ()+3)), *getQ() * *getQ() - *(getQ()+1) * *(getQ()+1)
                    - *(getQ()+2) * *(getQ()+2) + *(getQ()+3) * *(getQ()+3));
      myIMU.yaw   *= RAD_TO_DEG;
      // Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
      //   8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
      // - http://www.ngdc.noaa.gov/geomag-web/#declination
      myIMU.yaw   -= 8.5;
      myIMU.roll  *= RAD_TO_DEG;

    // Serial print and/or display at 0.5 s rate independent of data rates
    myIMU.delt_t = millis() - myIMU.count;

    // update LCD once per half-second independent of read rate
    if (myIMU.delt_t > 500)
    {
      if(SerialDebug)
      {
        
        Serial.print("Ax: ");
        Serial.print(myIMU.ax);
        Serial.print(" Ay: ");
        Serial.print(myIMU.ay);
        Serial.print(" Az: ");
        Serial.print(myIMU.az);
        Serial.print(" ");
        
        Serial.print("Gx: ");
        Serial.print(myIMU.gx);
        Serial.print(" Gy: ");
        Serial.print(myIMU.gy);
        Serial.print(" Gz: ");
        Serial.print(myIMU.gz);
        Serial.println(" ");
        
        Serial.print("Yaw, Pitch, Roll: ");
        Serial.print(myIMU.yaw, 2);
        Serial.print(", ");
        Serial.print(myIMU.pitch, 2);
        Serial.print(", ");
        Serial.println(myIMU.roll, 2);

        Serial.print("rate = ");
        Serial.print((float)myIMU.sumCount/myIMU.sum, 2);
        Serial.println(" Hz");
      }

      myIMU.count = millis();
      myIMU.sumCount = 0;
      myIMU.sum = 0;
    } // if (myIMU.delt_t > 500)
}

float updateTestPitch() {
  float dt = 0.01; // 10Hz
//  float accelPitch = -atan2(myIMU.ax, myIMU.az) * 180 / PI;
//  testPitch = 0.99 * (testPitch + myIMU.gz * dt) + 0.01 * accelPitch;
  float accelPitch = -atan2(myIMU.ax - accel_bias[0], myIMU.az - accel_bias[2]) * 180 / PI;
  testPitch = 0.99 * (testPitch + (myIMU.gy - gyro_bias[1])* dt) + 0.01 * accelPitch;

  if (testPitchCount > 10) {
    testPitchCount = 0;
    Serial.print("Accel only: ");
    Serial.print(accelPitch);
    Serial.print(" Gyro Value: ");
    Serial.print(myIMU.gy - gyro_bias[1]);
    Serial.print(" Final: ");
    Serial.println(testPitch);
  }
  testPitchCount++;
  return testPitch;
}

float getPitch() {
  return myIMU.pitch;
}
