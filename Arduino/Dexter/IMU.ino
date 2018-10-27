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

#include "MPU9250.h"

#define SerialDebug false  // Set to true to get Serial output for debugging

float accel_bias[3];
float gyro_bias[3];

MPU9250 myIMU;

int pitchCount = 0;

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

    myIMU.initMPU9250();
    // Initialize device for active mode read of acclerometer, gyroscope, and
    // temperature
    Serial.println("MPU9250 initialized for active data mode....");
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

  // Must be called before updating quaternions!
  myIMU.updateTime();

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

      Serial.print("rate = ");
      Serial.print((float)myIMU.sumCount/myIMU.sum, 2);
      Serial.println(" Hz");
    }

    myIMU.count = millis();
    myIMU.sumCount = 0;
    myIMU.sum = 0;
    } // if (myIMU.delt_t > 500)
}

float updatePitch() {
  float dt = 0.01; // 100Hz
  float accelPitch = -atan2(myIMU.ax - accel_bias[0], myIMU.az - accel_bias[2]) * 180 / PI;
  pitch = 0.99 * (pitch + (myIMU.gy - gyro_bias[1])* dt) + 0.01 * accelPitch;

  if (pitchCount > 10) {
    pitchCount = 0;
    Serial.print("Accel only: ");
    Serial.print(accelPitch);
    Serial.print(" Gyro Value: ");
    Serial.print(myIMU.gy - gyro_bias[1]);
    Serial.print(" Final: ");
    Serial.println(pitch);
  }
  pitchCount++;
  return pitch;
}
