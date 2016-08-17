#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Servo.h>
#include <CurieBLE.h>

int steps = 0;
int distance;
int pos = 0;    // variable to store the servo position

BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)
BLEService writeService("01db05c9-54f8-4ea9-b243-2177eba200b5");

// BLE characteristic - custom 128-bit UUID, read and writable by central
BLEIntCharacteristic writeCharacteristic("de46a385-d410-4f00-8b8d-ec116984841c", BLEWrite);

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *motorR = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *motorL = AFMS.getStepper(200, 2);

// U D L R
boolean Data[4] = {0, 0, 0, 0}; // U D L R , records whis buttons are pressed (1,0,0,0 forward pressed, 1010 forward and left)

const int MOVEMENTS[4][2] = {{1, 1}, { -1, -1}, {1, -1}, { -1, 1}}; //relates forward back... to motor movements.
//ie forward both motors move forward eg {1,1},
//left the left motor moves forward while the right moves back eg{1,-1}
int Move[2] = {0, 0}; //records the sum of movements if multiple buttons are pressed

void setup()
{
  Serial.begin(9600);
  while (!Serial);

  // set advertised local name and service UUID:
  blePeripheral.setLocalName("GENUINO 101-C575");
  blePeripheral.setAdvertisedServiceUuid(writeService.uuid());

  // add service and characteristic:
  blePeripheral.addAttribute(writeService);
  blePeripheral.addAttribute(writeCharacteristic);

  // set the initial value for the characeristic:
  writeCharacteristic.setValue(-1);

  // begin advertising BLE service:
  blePeripheral.begin();

  AFMS.begin();
  motorR->setSpeed(100);
  motorL->setSpeed(100);

  Serial.println("BLE write example");
}

void loop()
{
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // a variable to store written values
  int info = -1;

  // if a central is connected to peripheral:
  if (central)
  {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected())
    {
      // if the remote device wrote to the characteristic,
      // use the value to control the board:
      if (writeCharacteristic.written())
      {
        info = writeCharacteristic.value() + 256; //not sure why it's 256 off but adding 256 corrects it
        int i;
        for (i = 3; i >= 0; i--) // convert the int to binary
        {
          if (info >= pow(2, i))
          {
            Data[i] = 1;
            info -=  pow(2, i);
          }
          else
          {
            Data[i] = 0;
          }
        }

        Move[0] = 0;
        Move[1] = 0;
        int k;
        for ( k = 0; k < 4; k++)
        {
          if (Data[k] == 1)
          {
            Move[0] += MOVEMENTS[k][0];
            Move[1] += MOVEMENTS[k][1];
          }
        }  
        Serial.print(Move[0]);
        Serial.println(Move[1]);        
      }

      moveMotor(Move);
    }
    // when the central disconnects, print it out:
    Serial.print(("Disconnected from central: "));
    Serial.println(central.address());
  }
}

int moveMotor(int input[2])
{
  if (input[0] >= 1)
  {
    motorL->step(1, FORWARD, DOUBLE);  // 1 step = 1.8degrees
  }
  else if (input[0] <= -1)
  {
    motorL->step(1, BACKWARD, DOUBLE);  // 1 step = 1.8degrees
  }
  else
  {
    delay(9); // 9 millisec is how long moving one step would take, stops one wheel movinf faster when only moving one wheel at a time
  }

  if (input[1] >= 1)
  {
    motorR->step(1, BACKWARD, DOUBLE);  // 1 step = 1.8degrees
  }
  else if (input[1] <= -1)
  {
    motorR->step(1, FORWARD, DOUBLE);  // 1 step = 1.8degrees
  }
  else
  {
    delay(9);
  }
}
