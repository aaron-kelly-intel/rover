const int temperaturePin = A0;
const int humidityPin = A1;

const float SENSOR_SUPPLY_VOLTAGE = 3.3;
int humidityOutput = 0;
int tempOutput = 0;
float trueRH;
// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.print("Temp: ");
  tempOutput = getTemperature();
  lcd.print(tempOutput);
  lcd.print("C");

  lcd.setCursor(0,1);
  
  lcd.print("Humi: ");
  humidityOutput = getHumidity(tempOutput);
  lcd.print(humidityOutput);
  lcd.print("%");
  
  delay(2000);
  lcd.clear();
}

float getHumidity(float degreesCelsius){
   // read the value from the sensor:
  int humidityVoltage = analogRead(humidityPin);
  float voltage = humidityVoltage/1024.0 * SENSOR_SUPPLY_VOLTAGE; // convert to voltage value

  // convert the voltage to a relative humidity
  // - the equation is derived from the HIH-4030/31 datasheet
  // - it is not calibrated to your individual sensor
  //  Table 2 of the sheet shows the may deviate from this line
  float sensorRH = 161.0 * voltage / SENSOR_SUPPLY_VOLTAGE - 25.8;
  float trueRH = sensorRH / (1.0546 - 0.0026 * degreesCelsius); //temperature adjustment 

  return trueRH;
}

float getTemperature() {
    // read the value on AnalogIn pin 0
  // and store it in a variable
  int sensorVoltage = analogRead(temperaturePin);

  // send the 10-bit sensor value out the serial port
   Serial.print("sensor Value: ");
  Serial.print(trueRH);

  // convert the ADC reading to voltage
  float voltage = sensorVoltage * SENSOR_SUPPLY_VOLTAGE;
  voltage = voltage / 1024.0;

  // Send the voltage level out the Serial port
 Serial.print(", Volts: ");
  Serial.print(voltage);

  // convert the voltage to temperature in degrees C
  // the sensor changes 10 mV per degree
  // the datasheet says there's a 500 mV offset
  // ((volatge - 500mV) times 100)
  Serial.print(", degrees C: ");
  float temperature = (voltage - 0.5) * 100;
  Serial.println(temperature);

  return temperature;
}

