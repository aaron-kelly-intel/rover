/*
  ===============================================
  Example sketch for CurieIMU library for Intel(R) Curie(TM) devices.
  Copyright (c) 2015 Intel Corporation.  All rights reserved.

  Based on I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050
  class by Jeff Rowberg: https://github.com/jrowberg/i2cdevlib

  ===============================================
  I2Cdev device library code is placed under the MIT license
  Copyright (c) 2011 Jeff Rowberg

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  ===============================================
*/

#include "CurieIMU.h"
int ax, ay, az;         // accelerometer values
int gx, gy, gz;         // gyrometer values

int dir[3] = {0, 0, 0}; // 1 up / -1 down , 1 L / -1 R , 1 Forward , -1 backward

int axRaw, ayRaw, azRaw;         // accelerometer values
int gxRaw, gyRaw, gzRaw;



void setup() {
  Serial.begin(9600); // initialize Serial communication
  while (!Serial);    // wait for the serial port to open

  // initialize device
  CurieIMU.begin();

  if (CurieIMU.begin())
  {
    Serial.println("CurieIMU connection successful");
  }
  else
  {
    Serial.println("CurieIMU connection failed");
  }

  CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);
}
void loop() {
  // read raw accel/gyro measurements from device
  CurieIMU.readMotionSensor(axRaw, ayRaw, azRaw, gxRaw, gyRaw, gzRaw);

  if (azRaw > 0)
  {
    dir[0] = 1; //up
  }
  else
  {
    dir[0] = -1; // down
  }

  if (ayRaw < 0)
  {
    dir[1] = 1; // left
  }
  else
  {
    dir[1] = -1; // right
  }

  if (azRaw < 0)
  {
    dir[0] = 1; // forward
  }
  else
  {
    dir[0] = -1;
  }


  ax = convertRawAcceleration(axRaw);
  ay = convertRawAcceleration(ayRaw);
  az = convertRawAcceleration(azRaw);

  gx = convertRawGyro(gxRaw);
  gy = convertRawGyro(gyRaw);
  gz = convertRawGyro(gzRaw);


  // display tab-separated accel/gyro x/y/z values
  Serial.print("a/g:\t");
  //Serial.print(ax);
  Serial.print("\t");
  Serial.print(axRaw);
  Serial.print("\t");
  //Serial.print(ay);
  Serial.print("\t");
  Serial.print(ayRaw);
  Serial.print("\t");
  //Serial.print(az);
  Serial.print("\t");
  Serial.print(azRaw);
  Serial.print("\t");
  Serial.print(double((pow(axRaw, 2) + pow(ayRaw, 2) + pow(azRaw, 2)) ^ (1 / 3)));
  Serial.println();

}

int convertRawAcceleration(double aRaw)
{
  double a = (acos(aRaw / 16384.0) / 3.1416) * 180;
  if (aRaw < 0)
  {
    return abs(a) * -1;
  }
  else
  {
    return abs(a);
  }
}

float convertRawGyro(int gRaw)
{
  float g = gRaw;

  return g;
}


