#include <CurieBLE.h>

// constants
const int DELAYTIME = 1000;
const int PIN_INTRODUCTION = 0;
const int PIN_MOVEMENT = 1;
const int PIN_IMU = 2;
const int PIN_SENSORS = 3;
const int PIN_CLOSING = 4;

byte voiceValue = 0; // store value written to voice characteristic

BLEPeripheral blePeripheral;

// services
BLEService daveVoiceBoard("c42caaf0-95ac-4460-baff-d50db4e4f4f1");

// characteristics
BLEUnsignedCharCharacteristic daveVoice("de46a385-d410-4f00-8b8d-ec116984841c", BLEWrite);

void setup() {
  Serial.begin(9600);

  // set advertised local name and service UUID:
  blePeripheral.setLocalName("davev");
  blePeripheral.setAdvertisedServiceUuid(daveVoiceBoard.uuid());

  // add service and characteristics:
  blePeripheral.addAttribute(daveVoiceBoard);
  blePeripheral.addAttribute(daveVoice);

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
      ** VOICE TRIGGERS **
      ********************/
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (daveVoice.written()) {
        voiceValue = daveVoice.value();

        switch (voiceValue) {
          case 0:
            Serial.println("PLAY: startup");
            digitalWrite(PIN_INTRODUCTION, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_INTRODUCTION, HIGH);
            break;
          case 1:
            Serial.println("PLAY: movement");
            digitalWrite(PIN_MOVEMENT, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_MOVEMENT, HIGH);
            break;
          case 2:
            Serial.println("PLAY: IMU");
            digitalWrite(PIN_IMU, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_IMU, HIGH);
            break;
          case 3:
            Serial.println("PLAY: sensors");
            digitalWrite(PIN_SENSORS, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_SENSORS, HIGH);
            break;
          case 4:
            Serial.println("PLAY: closing");
            digitalWrite(PIN_CLOSING, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_CLOSING, HIGH);
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
