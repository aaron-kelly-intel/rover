#include <CurieBLE.h>

// constants
const int DELAYTIME = 1000;
const int PIN_STARTUP = 0;
const int PIN_GREETING = 1;
const int PIN_SICK = 2;
const int PIN_MOVE_MIND = 3;
const int PIN_SIGNOFF = 4;

byte voiceValue = 0; // store value written to voice characteristic

BLEPeripheral blePeripheral;

// services
BLEService daveVoiceBoard("c42caaf0-95ac-4460-baff-d50db4e4f4f1");

// characteristics
BLEUnsignedCharCharacteristic daveVoice("893fe933-4d22-4ebd-986a-08492c3144b4", BLEWrite);

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
            digitalWrite(PIN_STARTUP, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_STARTUP, HIGH);
            break;
          case 1:
            Serial.println("PLAY: movement");
            digitalWrite(PIN_GREETING, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_GREETING, HIGH);
            break;
          case 2:
            Serial.println("PLAY: IMU");
            digitalWrite(PIN_SICK, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_SICK, HIGH);
            break;
          case 3:
            Serial.println("PLAY: sensors");
            digitalWrite(PIN_MOVE_MIND, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_MOVE_MIND, HIGH);
            break;
          case 4:
            Serial.println("PLAY: closing");
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
