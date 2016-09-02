
#include <CurieBLE.h>


BLEPeripheral blePeripheral;       // BLE Peripheral Device (the board you're programming)
BLEService temperatureService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE Battery Service

// BLE Battery Level Characteristic"
BLEUnsignedCharCharacteristic temperatureCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214",  // standard 16-bit characteristic UUID
    BLERead | BLENotify);     // remote clients will be able to get notifications if this characteristic changes

int oldTemperatureLevel = 0;  // last battery level reading from analog input
long previousMillis = 0;  // last time the battery level was checked, in ms

void setup() {
  Serial.begin(9600);    // initialize serial communication
  pinMode(A0, INPUT);   // initialise temperature readings
  pinMode(13, OUTPUT);   // initialize the LED on pin 13 to indicate when a central is connected

  
  blePeripheral.setLocalName("GENUINO");
  blePeripheral.setAdvertisedServiceUuid(temperatureService.uuid());  // add the service UUID
  blePeripheral.addAttribute(temperatureService);   // Add the BLE Battery service
  blePeripheral.addAttribute(temperatureCharacteristic); // add the battery level characteristic
  temperatureCharacteristic.setValue(oldTemperatureLevel);   // initial value for this characteristic

 
  blePeripheral.begin();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(13, HIGH);

   
    while (central.connected()) {
      long currentMillis = millis();
      // if 200ms have passed, check the battery level:
      if (currentMillis - previousMillis >= 200) {
        previousMillis = currentMillis;
        updateTemperatureLevel();
      }
    }
    
    digitalWrite(13, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void updateTemperatureLevel() {
  
  float temperatureLevel = analogRead(A0);

  // convert the ADC reading to voltage
  float voltage = (temperatureLevel / 1024.0) * 3.3;

  // Send the voltage level out the Serial port

  // convert the voltage to temperature in degrees C
  // the sensor changes 10 mV per degree
  // the datasheet says there's a 500 mV offset
  // ((volatge - 500mV) times 100)
  temperatureLevel = (voltage - .5) * 100;

  if (temperatureLevel != oldTemperatureLevel) {      
    Serial.print("Temperature level % is now: "); 
    Serial.println(temperatureLevel);
    temperatureCharacteristic.setValue(temperatureLevel); 
    oldTemperatureLevel = temperatureLevel;           
  }
}
