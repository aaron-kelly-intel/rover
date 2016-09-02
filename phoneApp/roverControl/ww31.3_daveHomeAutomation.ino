#include <CurieBLE.h>
#include <Servo.h>

const int DELAYTIME = 1000;
const int PIN_STARTUP = 0;
const int PIN_GREETING = 1;
const int PIN_EMAIL = 2;
const int PIN_LED_DO = 3;
const int PIN_MOVE_MIND = 4;
const int PIN_SIGNOFF = 5;
const int PIN_SERVO = 13;
byte readValue = 0;

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

  // set the initial value for the characeristic:
  // daveGreeting.setValue(0);

  // begin advertising BLE service:
  blePeripheral.begin();

  // set all digital pins to HIGH
  for (int i = 0; i <= 13; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

  // setup servo
  myServo.attach(PIN_SERVO);
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
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (daveVoice.written()) {
        readValue = daveVoice.value();

        switch (readValue) {
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
            Serial.println("PLAY: email");
            digitalWrite(PIN_EMAIL, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_EMAIL, HIGH);
          case 3:
            Serial.println("PLAY: LED");
            digitalWrite(PIN_LED_DO, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_LED_DO, HIGH);
            break;
          case 4:
            Serial.println("PLAY: moveMind");
            digitalWrite(PIN_MOVE_MIND, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_MOVE_MIND, HIGH);
            break;
          case 5:
            Serial.println("PLAY: signoff");
            digitalWrite(PIN_SIGNOFF, LOW);
            delay(DELAYTIME);
            digitalWrite(PIN_SIGNOFF, HIGH);
            break;
        }
      }

      if (daveServo.written()) {
        int pos = 0;

        for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myServo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
          myServo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
      }
    }
    // when the central disconnects, print it out:
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
