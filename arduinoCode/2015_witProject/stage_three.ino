/* Stage three WIT project

Tweets temperature and whether office lights are on or off when button is pressed
Otherwise scans for tweets using #allon #alloff #checktemp #officelightson with #redlight_on #redlight_off features and same for #bluelight #greenlight and #orangelight
Acts on coloured light commands turning lights on/off separately or together
Responds to temperature command by tweeting current temperature
Responds to office lights command by tweeting whether the overhead lights are on or off
Slightly smoother and faster than previous versions
-tweets sensor info, reads tweets, responds and works slightly faster

*/

#include <Dhcp.h>
#include <Dns.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <SPI.h>

#define temperature A1
#define LDR A0

const int redled = 5;            // the number of the red LED digital pin
const int orangeled = 4;         // the number of the orange LED digital pin
const int blueled = 2;           // the number of the blue LED digital pin
const int greenled = 3;          // the number of the green LED digital pin
const int buttonled = 6;         // the number of the button's LED digital pin
const int button = 7;            // the number of the pushbutton digital pin

// Local Network Settings
char ssid[] = "Ciara";           // your network SSID (name)
char pass[] = "11111111";        // your network password
/*char ssid[] = "E";             // your network SSID (name) 
char pass[] = "00000000";        // your network password*/
int status = WL_IDLE_STATUS;     // the Wifi radio's status

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String thingtweetAPIKey = "UT1NKTYRI172P8IU";

// Variable Setup
boolean lastConnected = false;
long lastConnectionTime = 0;            // record of last time board was connected to wifi
unsigned long nextPoll = 0;             // timer between checking posts
int failedCounter = 0;                  // record of how many times connection has failed
int buttonState = 0;                    // variable for reading pushbutton status
int light_value;                        // variable for reading LDR
int temperature_value;                  // variable for reading temperature sensor
float cel;                              // variable to calculate celcius value
char HTTPresponseCharacter;
char buf[9];                            // time array
String tweet = "";                      // phrase that will be tweeted
String HTTPresponse;                    // string sent to galileo from twitter
String ledStat = "";                    // status of coloured LEDs
String lastStat;                        // last status read
String lightson = "The lights are on. ";
String lightsoff = "The lights are off. ";

WiFiClient client;                      // initialize Arduino Wifi Client

void setup() {
  Serial.begin(115200);
  startWiFi();                                        // call wifi function
  pinMode(redled, OUTPUT);                            // define LED pins as outputs
  pinMode(blueled, OUTPUT);
  pinMode(orangeled, OUTPUT);
  pinMode(greenled, OUTPUT);
  pinMode(buttonled, OUTPUT);
  pinMode(button, INPUT);                             // set button pin as input
  system("date '+%H:%M:%S' > /home/root/time.txt");   // set the date/time to the board clock
  switchLight(0);                                    // turn colour LEDs off
}

void loop() {
  //Serial.print("start of loop");
  //Serial.print(millis());
  //Serial.print("\n");
  String tweet = "";                    // clear tweet string
  get_time();
  buttonState = digitalRead(button);    // check if button is pressed

  // check if button is pressed and act accordingly
  if (buttonState == LOW) {
    digitalWrite(buttonled, HIGH);
    check_temp();
    tweet += String(check_lights()) + "The temperature is " + String(int(cel)) + "." + String(getDecimal(cel)) + "*C " + "@ " + buf;
    updateTwitterStatus(tweet);
    delay(1000);
    String tweet = "";                    // clear tweet string
    if (client.connected() && lastConnected)
    {
      client.stop();
    }
  }
  else {
    digitalWrite(buttonled, LOW);
    // scan twitter for instructions
    if (nextPoll <= millis()) {
      nextPoll = millis() + 2000;
      connectToThingspeak();
      if (client.connected()) {
        failedCounter = 0;
      }
      else {
        failedCounter++;
        Serial.println("Connection failed (" + String(failedCounter, DEC) + ")");
      }
    }
    if (client.available()) {
      readHTTP();
    }
  }
  if (failedCounter > 3 ) {// Check if Wifi needs to be restarted
    startWiFi();
    failedCounter = 0;
  }
  lastConnected = client.connected();
}

void startWiFi() {
  String fv = WiFi.firmwareVersion();
  if ( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);    // Connect to WPA/WPA2 network
    delay(1000);
  }
  if (status == WL_CONNECTED) {
  Serial.print("Connected to the network\n");
  }
}

void connectToThingspeak() {
  if (client.connect("api.thingspeak.com", 80)) {
    client.println("GET /channels/47866/field/1/last.txt HTTP/1.0"); //Change CHANNEL_ID to the ID of your channel
    client.println();
    //Serial.println("Connected to channel");
  }
  HTTPresponse = "";
}

void readStatus() {
  while (client.available()) {
    char HTTPresponseCharacter = client.read();
    ledStat += HTTPresponseCharacter;
  }
  client.stop();
  ledStat.trim();
  //Serial.println(ledStat);
  if (ledStat == "redlight_on") {
    Serial.println("red LED is on");
    switchLight(1);
    lastStat = ledStat;
  }
  else if (ledStat == "redlight_off") {
    Serial.println("red LED is off");
    switchLight(-1);
    lastStat = ledStat;
  }
  else if (ledStat == "orangelight_on") {
    Serial.println("orange LED is on");
    switchLight(2);
    lastStat = ledStat;
  }
  else if (ledStat == "orangelight_off") {
    Serial.println("orange LED is off");
    switchLight(-2);
    lastStat = ledStat;
  }
  else if (ledStat == "greenlight_on") {
    Serial.println("green LED is on");
    switchLight(4);
    lastStat = ledStat;
  }
  else if (ledStat == "greenlight_off") {
    Serial.println("green LED is off");
    switchLight(-4);
    lastStat = ledStat;
  }
  else if (ledStat == "bluelight_on") {
    Serial.println("blue LED is on");
    switchLight(5);
    lastStat = ledStat;
  }
  else if (ledStat == "bluelight_off") {
    Serial.println("blue LED is off");
    switchLight(-5);
    lastStat = ledStat;
  }
  else if (ledStat == "alloff") {
    Serial.println("all LEDs are off");
    switchLight(0);
    lastStat = ledStat;
  }
  else if (ledStat == "allon") {
    Serial.println("all LEDs are on");
    switchLight(100);
    lastStat = ledStat;
  }
  else if (ledStat == "checktemp" && lastStat != "checktemp") {
    Serial.println("Checking temperature");
    check_temp();
    tweet += "The temperature is " + String(int(cel)) + "." + String(getDecimal(cel)) + "*C " + "@ " + buf;
    updateTwitterStatus(tweet);
    tweet = "";
    lastStat = ledStat;
  }
  else if (ledStat == "officelightson" && lastStat != "officelightson") {
    Serial.println("Checking brightness");
    analogRead(LDR);
    tweet += "The temperature is " + String(int(cel)) + "." + String(getDecimal(cel)) + "*C " + "@ " + buf;
    updateTwitterStatus(tweet);
    tweet = "";
    lastStat = ledStat;
  }
  else {
    Serial.println("Didn't find the status");
  }
  ledStat = "";
}

void switchLight (int ledStatus) {
  switch (ledStatus) {                  // case number corresponds to number in the rainbow
    case 1:
      digitalWrite(redled, HIGH);
      break;
    case -1:
      digitalWrite(redled, LOW);
      break;
    case 2:
      digitalWrite(orangeled, HIGH);
      break;
    case -2:
      digitalWrite(orangeled, LOW);
      break;
    case 4:
      digitalWrite(greenled, HIGH);
      break;
    case -4:
      digitalWrite(greenled, LOW);
      break;
    case 5:
      digitalWrite(blueled, HIGH);
      break;
    case -5:
      digitalWrite(blueled, LOW);
      break;
    case 0:
      digitalWrite(redled, LOW);
      digitalWrite(orangeled, LOW);
      digitalWrite(blueled, LOW);
      digitalWrite(greenled, LOW);
      break;
    case 100:
      digitalWrite(redled, HIGH);
      digitalWrite(orangeled, HIGH);
      digitalWrite(blueled, HIGH);
      digitalWrite(greenled, HIGH);
      break;
  }
}

void updateTwitterStatus(String tsData){
  if (client.connect(thingSpeakAddress, 80)){
    Serial.print("updating twitter status\n");
    // Create HTTP POST Data
    tsData = "api_key=" + thingtweetAPIKey + "&status=" + tsData;

    client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");
    client.print(tsData);
    lastConnectionTime = millis();
  }
  else {
    failedCounter++;
    //Serial.print("entered update twitter status else\n");
    Serial.println("Connection to ThingSpeak Failed (" + String(failedCounter, DEC) + ")");
    lastConnectionTime = millis();
  }
}

// function to calculate temperature
void check_temp() {
  temperature_value = analogRead(temperature);        // check value from sensor
  float mv = ( temperature_value / 1024.0) * 5000;    // use value to calculate temperature
  float farh = mv / 10;
  cel = ((farh - 32) * 5) / 9;
}

// function to check if lights are on/off and print to serial monitor
String check_lights() {
  light_value = analogRead(LDR);                      // read value from ldr
  if (light_value < 50) {
    return lightsoff;                  // low value means lights are off
  }
  else {
    return lightson;                   // high value means lights are on
  }
}

void get_time() {
  system("date '+%H:%M:%S' > /home/root/time.txt");  //get current time in the format hours:minutes:secs and save in time.txt in /home/root
  FILE *fp;
  fp = fopen("/home/root/time.txt", "r");
  fgets(buf, 9, fp);
  fclose(fp);
}

void readHTTP() {
  char HTTPresponseCharacter = client.read();
  HTTPresponse += HTTPresponseCharacter;
  if (HTTPresponseCharacter == '\n') { // find the end of the http response ie. when char is \n
    if (HTTPresponse.length() == 2) {
      Serial.print("reading status...\n");
      //Serial.print(millis());
      readStatus();
      //Serial.print("finished reading status...\n");
      //Serial.print(millis());
    }
    HTTPresponse = "";
  }
}

long getDecimal(float val) {
  int intPart = int(val);
  long decPart = 1000 * (val - intPart);               //multiplying by 1000 assuming foat values have max of 3 decimal places
  //Change to match the number of decimal places you need
  if (decPart > 0)return (decPart);                    //return the decimal part of float number if it is available
  else if (decPart < 0)return ((-1) * decPart);        //if negative, multiply by -1
  else if (decPart = 0)return (00);                    //return 0 if decimal part of float number is not available
}
