#include "Wire.h"

// MPU6050 Register map
#define MPU6050_SMPLRT_DIV         0x19   // R/W
#define MPU6050_CONFIG             0x1A   // R/W
#define MPU6050_GYRO_CONFIG        0x1B   // R/W
#define MPU6050_ACCEL_CONFIG       0x1C   // R/W
#define MPU6050_INT_ENABLE         0x38   // R/W
#define MPU6050_INT_STATUS         0x3A   // R  
#define MPU6050_ACCEL_XOUT_H       0x3B   // R  
#define MPU6050_PWR_MGMT_1         0x6B   // R/W
#define MPU6050_WHO_AM_I           0x75   // R

// Defines for the bits, to be able to change
// between bit number and binary definition.
// By using the bit number, programming the sensor
// is like programming the AVR microcontroller.
// But instead of using "(1<<X)", or "_BV(X)",
// the Arduino "bit(X)" is used.
#define MPU6050_D0 0
#define MPU6050_D1 1
#define MPU6050_D2 2
#define MPU6050_D3 3
#define MPU6050_D4 4
#define MPU6050_D5 5
#define MPU6050_D6 6
#define MPU6050_D7 7

// AUX_VDDIO Register
#define MPU6050_AUX_VDDIO MPU6050_D7  // I2C high: 1=VDD, 0=VLOGIC

// CONFIG Register
// DLPF is Digital Low Pass Filter for both gyro and accelerometers.
// These are the names for the bits.
// Use these only with the bit() macro.
#define MPU6050_DLPF_CFG0     MPU6050_D0
#define MPU6050_DLPF_CFG1     MPU6050_D1
#define MPU6050_DLPF_CFG2     MPU6050_D2
#define MPU6050_EXT_SYNC_SET0 MPU6050_D3
#define MPU6050_EXT_SYNC_SET1 MPU6050_D4
#define MPU6050_EXT_SYNC_SET2 MPU6050_D5

// Combined definitions for the DLPF_CFG values
#define MPU6050_DLPF_CFG_0 (0)
#define MPU6050_DLPF_CFG_1 (bit(MPU6050_DLPF_CFG0))
#define MPU6050_DLPF_CFG_2 (bit(MPU6050_DLPF_CFG1))
#define MPU6050_DLPF_CFG_3 (bit(MPU6050_DLPF_CFG1)|bit(MPU6050_DLPF_CFG0))
#define MPU6050_DLPF_CFG_4 (bit(MPU6050_DLPF_CFG2))
#define MPU6050_DLPF_CFG_5 (bit(MPU6050_DLPF_CFG2)|bit(MPU6050_DLPF_CFG0))
#define MPU6050_DLPF_CFG_6 (bit(MPU6050_DLPF_CFG2)|bit(MPU6050_DLPF_CFG1))
#define MPU6050_DLPF_CFG_7 (bit(MPU6050_DLPF_CFG2)|bit(MPU6050_DLPF_CFG1)|bit(MPU6050_DLPF_CFG0))

// Alternative names for the combined definitions
// This name uses the bandwidth (Hz) for the accelometer,
// for the gyro the bandwidth is almost the same.
#define MPU6050_DLPF_260HZ    MPU6050_DLPF_CFG_0
#define MPU6050_DLPF_184HZ    MPU6050_DLPF_CFG_1
#define MPU6050_DLPF_94HZ     MPU6050_DLPF_CFG_2
#define MPU6050_DLPF_44HZ     MPU6050_DLPF_CFG_3
#define MPU6050_DLPF_21HZ     MPU6050_DLPF_CFG_4
#define MPU6050_DLPF_10HZ     MPU6050_DLPF_CFG_5
#define MPU6050_DLPF_5HZ      MPU6050_DLPF_CFG_6
#define MPU6050_DLPF_RESERVED MPU6050_DLPF_CFG_7

// GYRO_CONFIG Register
// The XG_ST, YG_ST, ZG_ST are bits for selftest.
// The FS_SEL sets the range for the gyro.
// These are the names for the bits.
// Use these only with the bit() macro.
#define MPU6050_FS_SEL0 MPU6050_D3
#define MPU6050_FS_SEL1 MPU6050_D4
#define MPU6050_ZG_ST   MPU6050_D5
#define MPU6050_YG_ST   MPU6050_D6
#define MPU6050_XG_ST   MPU6050_D7

// Combined definitions for the FS_SEL values
#define MPU6050_FS_SEL_0 (0)
#define MPU6050_FS_SEL_1 (bit(MPU6050_FS_SEL0))
#define MPU6050_FS_SEL_2 (bit(MPU6050_FS_SEL1))
#define MPU6050_FS_SEL_3 (bit(MPU6050_FS_SEL1)|bit(MPU6050_FS_SEL0))

// Alternative names for the combined definitions
// The name uses the range in degrees per second.
#define MPU6050_FS_SEL_250  MPU6050_FS_SEL_0
#define MPU6050_FS_SEL_500  MPU6050_FS_SEL_1
#define MPU6050_FS_SEL_1000 MPU6050_FS_SEL_2
#define MPU6050_FS_SEL_2000 MPU6050_FS_SEL_3

// Combined definitions for the AFS_SEL values
#define MPU6050_AFS_SEL_0 (0)
#define MPU6050_AFS_SEL_1 (bit(MPU6050_AFS_SEL0))
#define MPU6050_AFS_SEL_2 (bit(MPU6050_AFS_SEL1))
#define MPU6050_AFS_SEL_3 (bit(MPU6050_AFS_SEL1)|bit(MPU6050_AFS_SEL0))

// Alternative names for the combined definitions
// The name uses the full scale range for the accelerometer.
#define MPU6050_AFS_SEL_2G  MPU6050_AFS_SEL_0
#define MPU6050_AFS_SEL_4G  MPU6050_AFS_SEL_1
#define MPU6050_AFS_SEL_8G  MPU6050_AFS_SEL_2
#define MPU6050_AFS_SEL_16G MPU6050_AFS_SEL_3

// INT_ENABLE Register
// These are the names for the bits.
// Use these only with the bit() macro.
#define MPU6050_DATA_RDY_EN    MPU6050_D0

// PWR_MGMT_1 Register
// These are the names for the bits.
// Use these only with the bit() macro.
#define MPU6050_DEVICE_RESET MPU6050_D7    // 1: reset to default values

// Default I2C address for the MPU-6050 is 0x68.
#define MPU6050_I2C_ADDRESS 0x68

// Util function to swap byte values
uint8_t swap;
#define SWAP(x,y) swap = x; x = y; y = swap

// Declaring an union for the registers and the axis values.
// The byte order does not match the byte order of
// the compiler and AVR chip.
// The AVR chip (on the Arduino board) has the Low Byte
// at the lower address.
// But the MPU-6050 has a different order: High Byte at
// lower address, so that has to be corrected.
// The register part "reg" is only used internally,
// and are swapped in code.
typedef union accel_t_gyro_union
{
  struct
  {
    uint8_t x_accel_h;
    uint8_t x_accel_l;
    uint8_t y_accel_h;
    uint8_t y_accel_l;
    uint8_t z_accel_h;
    uint8_t z_accel_l;
    uint8_t t_h;
    uint8_t t_l;
    uint8_t x_gyro_h;
    uint8_t x_gyro_l;
    uint8_t y_gyro_h;
    uint8_t y_gyro_l;
    uint8_t z_gyro_h;
    uint8_t z_gyro_l;
  } reg;
  struct
  {
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t temperature;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
  } value;
};

#define SerialDebug false  // Set to true to get Serial output for debugging
#define ACCEL_RES 8192.0 // LSB/mg
#define GYRO_RES 65.5 // LSB/deg/s

float accel_bias[3];
float gyro_bias[3];

// Global MPU6050 IMU variables
accel_t_gyro_union accel_t_gyro;

int pitchCount = 0;

void MPU6050_setup()
{
  int error;
  uint8_t c;

  Wire.begin();
  error = MPU6050_read(MPU6050_WHO_AM_I, &c, 1);
  Serial.print("WHO_AM_I : ");
  Serial.print(c, HEX);
  Serial.print(", error = ");
  Serial.println(error, DEC);

  // RESET chip
  MPU6050_write_reg(MPU6050_PWR_MGMT_1, bit(MPU6050_DEVICE_RESET));
  delay(125);
  // Clear the 'sleep' bit to start the sensor and select clock source
  MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0x01);
  //MPU6050_write_reg(MPU6050_PWR_MGMT_1,MPU6050_CLKSEL_Z);

  // Config Gyro scale (500deg/seg)
  MPU6050_write_reg(MPU6050_GYRO_CONFIG, MPU6050_FS_SEL_500);
  // Config Accel scale (2g)
  MPU6050_write_reg(MPU6050_ACCEL_CONFIG, MPU6050_AFS_SEL_2G);
  // Config Digital Low Pass Filter 10Hz
  MPU6050_write_reg(MPU6050_CONFIG, MPU6050_DLPF_10HZ);
  // Set Sample Rate to 100Hz
  MPU6050_write_reg(MPU6050_SMPLRT_DIV, 9);  // 100Hz : Sample Rate = 1000 / (1 + SMPLRT_DIV) Hz
  // Data ready interrupt enable
  MPU6050_write_reg(MPU6050_INT_ENABLE, MPU6050_DATA_RDY_EN);
  // Clear the 'sleep' bit to start the sensor (and select clock source).
  MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0x01);
}

void calibrateImu() {
  // read accel and gyro for 2 seconds and subtract the results from subsequent measurements

  long startTimeMs = millis();
  int count = 0;

  while(millis() - startTimeMs < 2000) {
    MPU6050_read_3axis();
    accel_bias[0] += accel_t_gyro.value.x_accel / ACCEL_RES;
    accel_bias[1] += accel_t_gyro.value.y_accel / ACCEL_RES;
    accel_bias[2] += accel_t_gyro.value.z_accel / ACCEL_RES - 2;

    gyro_bias[0] += accel_t_gyro.value.x_gyro / GYRO_RES;
    gyro_bias[1] += accel_t_gyro.value.y_gyro / GYRO_RES;
    gyro_bias[2] += accel_t_gyro.value.z_gyro / GYRO_RES;
    
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

float updatePitch() {
  MPU6050_read_3axis();
  float dt = 0.005; // 200Hz
  float alpha = 0.01;
  float accelPitch = -atan2(accel_t_gyro.value.x_accel / ACCEL_RES - accel_bias[0], accel_t_gyro.value.z_accel / ACCEL_RES - accel_bias[2]) * 180 / PI;
  pitch = (1 - alpha) * (pitch + (accel_t_gyro.value.y_gyro / GYRO_RES - gyro_bias[1])* dt) + alpha * accelPitch;

  if (pitchCount > 25) {
    pitchCount = 0;
//    Serial1.print(millis());
//    Serial1.print('\t');
//    Serial1.print(accel_t_gyro.value.x_accel / ACCEL_RES - accel_bias[0]);
//    Serial1.print('\t');
//    Serial1.print(accel_t_gyro.value.z_accel / ACCEL_RES - accel_bias[2]);
//    Serial1.print('\t');
//    Serial1.print(accel_t_gyro.value.y_gyro / GYRO_RES - gyro_bias[1]);
//    Serial1.print('\t');
//    Serial1.print(accelPitch);
//    Serial1.print('\t');
//    Serial1.println(pitch);
  }
  pitchCount++;
  return pitch;
}

void MPU6050_read_3axis()
{
  int error;

  // read 14 bytes (gyros, temp and accels)
  error = MPU6050_read (MPU6050_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro));
  if (error != 0) {
    Serial.print("MPU6050 Error:");
    Serial.println(error);
  }
  // swap bytes
  SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
  SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
  SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
  SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
  SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
  SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
  SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l); 
}

// return true on new data available
bool MPU6050_newData()
{
  uint8_t status;
  int error;

  error = MPU6050_read(MPU6050_INT_STATUS, &status, 1);
  if (error != 0) {
    Serial.print("MPU6050 Error:");
    Serial.println(error);
  }
  if (status & (0b00000001)) // Data ready?
    return true;
  else
    return false;
}

// MPU6050_read n bytes
int MPU6050_read(int start, uint8_t *buffer, int size)
{
  int i, n;
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);
  if (n != 1)
    return (-10);

  n = Wire.endTransmission(false);    // hold the I2C-bus
  if (n != 0)
    return (n);

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  i = 0;
  while (Wire.available() && i < size)
  {
    buffer[i++] = Wire.read();
  }
  if ( i != size)
    return (-11);

  return (0);  // return : no error
}


// MPU6050_write n bytes
int MPU6050_write(int start, const uint8_t *pData, int size)
{
  int n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);        // write the start address
  if (n != 1)
    return (-20);

  n = Wire.write(pData, size);  // write data bytes
  if (n != size)
    return (-21);

  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0) {
    return (error);
  }

  return (0);         // return : no error
}

// --------------------------------------------------------
// MPU6050_write_reg (only 1 byte)
int MPU6050_write_reg(int reg, uint8_t data)
{
  int error;

  error = MPU6050_write(reg, &data, 1);

  return (error);
}
