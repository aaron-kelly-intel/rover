/*
  Read-a-Tweet v7

  Based on CheerLights by ioBridge Labs - www.cheerlights.com
  Additions and edits by Anouska de Graaf - www.anouskadegraaf.nl

*/
#include <Dhcp.h>
#include <Dns.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <SPI.h>

char ssid[] = "Ciara";           // your network SSID (name)
char pass[] = "11111111";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

WiFiClient client;

int redled = 2; //LED pin - change this pin to whatever pin your LED is connected to
int blueled = 3;
int greenled = 4;
String responseLine;
String ledStat;
unsigned long nextPoll = 0;

void setup() {
  Serial.begin(9600);
  pinMode(redled, OUTPUT); //Define each pin
  pinMode(blueled, OUTPUT); //Define each pin
  pinMode(greenled, OUTPUT); //Define each pin
  startWiFi();
  digitalWrite(redled, LOW);
  digitalWrite(blueled, LOW);
  digitalWrite(greenled, LOW);
}

void loop() {
  if (nextPoll <= millis()) {
    nextPoll = millis() + 10000;
    connectToTwitter();
  };
  if (client.available()) {
    char ch = client.read();
    responseLine += ch;
    Serial.print(ch);
    //if end of line found
    if (ch == 10) {
      Serial.println(responseLine.length());
      if (responseLine.length() == 2) {
        Serial.print("reading status..........");
        readStatus();
      };
      responseLine = "";
    };
  };
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    Serial.print("got: ");
    Serial.println(cmd);
    switchLight(cmd);
  };
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
    delay(5000);
  }
  Serial.print("You're connected to the network\n");
}

void connectToTwitter() {
  Serial.println("Connecting to Thingspeak...");
  if (client.connect("api.thingspeak.com", 80)) {
    Serial.println("Connected to API...");
    client.println("GET /channels/47866/field/1/last.txt HTTP/1.0"); //Change CHANNEL_ID to the ID of your channel
    client.println();
    Serial.println("Connected to channel. Good job!");
  }
  else {
    Serial.println("Connection failed");
  }
  responseLine = "";
  ledStat = "";
}

void readStatus() {
  //Serial.println("Entering readStatus");
  while (client.available()) {
    char ch = client.read();
    ledStat += ch;
  };
  client.stop();
  ledStat.trim();
  Serial.println(ledStat);
  if (ledStat == "redlight") { //Change 'lampaan' to your trigger!!!
    Serial.println("red LED is on");
    switchLight(49);
  }
  else if (ledStat == "bluelight") { //Change 'lampuit' to your trigger!!!
    Serial.println("blue LED is on");
    switchLight(50);
  }
  else if (ledStat == "greenlight") { //Change 'lampaan' to your trigger!!!
    Serial.println("green LED is on");
    switchLight(51);
  }
  else if (ledStat == "alloff") {
    Serial.println("all LEDs are off");
    switchLight(52);
  }
  else if (ledStat == "allon") {
    Serial.println("all LEDs are on");
    switchLight(53);
  }
  else {
    Serial.println("Didn't find the status");
    Serial.println(ledStat);
    Serial.println(ledStat.length());
  };
}

void switchLight (int ledStatus) {
  switch (ledStatus) {
    case 49:
      digitalWrite(redled, HIGH);
      break;
    case 50:
      digitalWrite(blueled, HIGH);
      break;
    case 51:
      digitalWrite(greenled, HIGH);
      break;
    case 52:
      digitalWrite(redled, LOW);
      digitalWrite(blueled, LOW);
      digitalWrite(greenled, LOW);
      break;
    case 53:
      digitalWrite(redled, HIGH);
      digitalWrite(blueled, HIGH);
      digitalWrite(greenled, HIGH);
      break;
      /*default:
        digitalWrite(redled, LOW);
        digitalWrite(blueled, LOW);
        digitalWrite(greenled, LOW);*/
  };
}
