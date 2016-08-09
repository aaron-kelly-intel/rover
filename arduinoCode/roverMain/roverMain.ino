#include <CurieBLE.h>
#include <CurieIMU.h>
#include <Servo.h>

// constants
const int DELAYTIME = 1000;
const int PIN_STARTUP = 0;
const int PIN_GREETING = 1;
const int PIN_SICK = 2;
const int PIN_MOVE_MIND = 3;
const int PIN_SIGNOFF = 4;

// raw gyro values
int gxRaw, gyRaw, gzRaw;
float gx, gy, gz;

byte voiceValue = 0; // store value written to voice characteristic

BLEPeripheral blePeripheral;
Servo myServo;

// services
BLEService daveBoard("c42caaf0-95ac-4460-baff-d50db4e4f4f1");

// characteristics
BLEUnsignedCharCharacteristic daveVoice("893fe933-4d22-4ebd-986a-08492c3144b4", BLEWrite);
BLEUnsignedCharCharacteristic daveServo("b2cbbd52-f3c2-4c88-924f-9f254d4c0e45", BLEWrite);

void setup() {
  Serial.begin(9600);

  // set advertised local name and service UUID:
  blePeripheral.setLocalName("daveb");
  blePeripheral.setAdvertisedServiceUuid(daveBoard.uuid());

  // add service and characteristics:
  blePeripheral.addAttribute(daveBoard);
  blePeripheral.addAttribute(daveVoice);
  blePeripheral.addAttribute(daveServo);

  // initialise IMU
  Serial.println("Initializing IMU device...");
  CurieIMU.begin();
  // Set the accelerometer range to 250 degrees/second
  CurieIMU.setGyroRange(250);

  // begin advertising BLE service:
  blePeripheral.begin();

  // set all digital pins to HIGH
  for (int i = 0; i <= 13; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
}

void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {

      /********************
      ******** IMU ********
      ********************/

      // read raw gyro measurements from device
      CurieIMU.readGyro(gxRaw, gyRaw, gzRaw);

      // convert the raw gyro data to degrees/second
      gx = convertRawGyro(gxRaw);
      gy = convertRawGyro(gyRaw);
      gz = convertRawGyro(gzRaw);

      // display tab-separated gyro x/y/z values
      Serial.print("g:\t");
      Serial.print(gx);
      Serial.print("\t");
      Serial.print(gy);
      Serial.print("\t");
      Serial.print(gz);
      Serial.println();

      if ((abs(gx) > 100) || (abs(gy) > 100) || (abs(gz) > 100)) {
        Serial.println("PLAY: sick");
        digitalWrite(PIN_SICK, LOW);
        delay(DELAYTIME);
        digitalWrite(PIN_SICK, HIGH);
      }

      /********************
      ** VOICE TRIGGERS **
      ********************/
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (daveVoice.written()) {
        voiceValue = daveVoice.value();

        switch (voiceValue) {
          case 0:
            Serial.println("PLAY: startup");
            digitalWrite(PIN_STARTUP, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_STARTUP, HIGH);
            break;
          case 1:
            Serial.println("PLAY: greeting");
            digitalWrite(PIN_GREETING, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_GREETING, HIGH);
            break;
          case 2:
            Serial.println("PLAY: sick");
            digitalWrite(PIN_SICK, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_SICK, HIGH);
            break;
          case 3:
            Serial.println("PLAY: moveMind");
            digitalWrite(PIN_MOVE_MIND, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_MOVE_MIND, HIGH);
            break;
          case 4:
            Serial.println("PLAY: signoff");
            digitalWrite(PIN_SIGNOFF, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_SIGNOFF, HIGH);
            break;
        }
      }
      // wait a while before the next readings
      delay(DELAYTIME);
    }
    // when the central disconnects, print it out:
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767
  float g = (gRaw * 250.0) / 32768.0;
  return g;
}
