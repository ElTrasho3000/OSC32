// include libraries
#include <Arduino.h>
#include <WiFi.h>
#include <OSCMessage.h>
#include <ESPAsyncWebServer.h>
#include <BluetoothSerial.h>
#include <SPIFFS.h>
// link other files
#include "wifiSetup.h"
#include "oscTerminal.h"
#include "startStationMode.h"

// GPIO 14 as a switch to toggle setup-mode.
const unsigned int SETUP_SWITCH = 14;

// GPIOs for analog input 
const unsigned int A_IN1 = 34;
const unsigned int A_IN2 = 35;
const unsigned int A_IN3 = 32;
const unsigned int A_IN4 = 33;

// GPIOs for LED
const unsigned int LEDR = 25;
const unsigned int LEDG = 26;
const unsigned int LEDB = 27; 

// Array of unused GPIOs
const unsigned int UNUSED_GPIO[12] {
  12, 13, 23, 22, 21, 4, 0, 2, 15, 19, 18, 5
};

// Variables to store and compare input values
unsigned int prevValA_IN1 = 0;
unsigned int prevValA_IN2 = 0;
unsigned int prevValA_IN3 = 0;
unsigned int prevValA_IN4 = 0;

void setup() {
  // Set pulldown for unused pins
  for (int i = 0; i <= 11; i++) {
    pinMode(UNUSED_GPIO[i], INPUT_PULLUP);
  };
  // Set analog input modes
  pinMode(A_IN1, INPUT_PULLDOWN);
  pinMode(A_IN2, INPUT_PULLDOWN);
  pinMode(A_IN3, INPUT_PULLDOWN); 
  pinMode(A_IN4, INPUT_PULLDOWN);
  // Set LED GPIOs to output and 0
  pinMode(LEDR, OUTPUT);
  digitalWrite(LEDR, 0);
  pinMode(LEDG, OUTPUT);
  digitalWrite(LEDG, 0);
  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDB, 0);
  // Use internal pulldown-resistor to set low.
  pinMode(SETUP_SWITCH, INPUT_PULLDOWN);
  // Start Serial Monitor
  Serial.begin(9600);
  Serial.println("Serial started");
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // Check whether Setup is enabled via switch
  if (digitalRead(SETUP_SWITCH) == 1) {
      while (wifiSetup() == true){
        digitalWrite(LEDR, 1);
        delay(1000);
      };
      digitalWrite(LEDR, 0);
      // Should we automatically put esp32 into deep sleep mode here?
  }
  // Connect to user's network
  startStationMode();
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LEDG, 1);
    delay(1000);
    digitalWrite(LEDG, 0);
    delay(1000);
  }
  // Initiate OSC-Terminal
  oscTerminal();
}

void loop(){
  while (UdpConfig.setup == 0){
    digitalWrite(LEDB, 1);
    delay(500);
    digitalWrite(LEDB, 0);
    delay(500);
  }
  // UDP target IP
  IPAddress tIP(UdpConfig.tip1, UdpConfig.tip2, UdpConfig.tip3, UdpConfig.tip4);
  // UDP sending port
  const unsigned int outPort = UdpConfig.outPort;
  // UDO target port
  const unsigned int tPort = UdpConfig.tPort;
  // Begin UDP
  Udp.begin(outPort);
  // Run UDP/OSC while
  while (UdpConfig.setup == 1){
    digitalWrite(LEDB, 1);
    unsigned int valA_IN1 = map((analogRead(A_IN1)/100), 0, 40, 0, 100);
    unsigned int valA_IN2 = map((analogRead(A_IN2)/100), 0, 40, 0, 100);
    unsigned int valA_IN3 = map((analogRead(A_IN3)/100), 0, 40, 0, 100);
    unsigned int valA_IN4 = map((analogRead(A_IN4)/100), 0, 40, 0, 100);
    // Wait until user configured UDP
    while (UdpConfig.a1 == 1 && valA_IN1 != prevValA_IN1){
      Udp.beginPacket(tIP, tPort);
      //the message takes an address as an required argument
      OSCMessage msg1("/ST");
      //add data of any type to the end of the message with 'add'";
      msg1.add("A1").add(valA_IN1);
      msg1.send(Udp);
      Udp.endPacket();
      msg1.empty();
      prevValA_IN1 = valA_IN1;
    }
    if (UdpConfig.a2 == 1 && valA_IN2 != prevValA_IN2){
      Udp.beginPacket(tIP, tPort);
      //the message takes an address as an required argument
      OSCMessage msg2("/ST");
      //add data of any type to the end of the message with 'add'";
      msg2.add("A2").add(valA_IN2);
      msg2.send(Udp);
      Udp.endPacket();
      msg2.empty();
      prevValA_IN2 = valA_IN2;
    }
    if (UdpConfig.a3 ==1 && valA_IN3 != prevValA_IN3){
      Udp.beginPacket(tIP, tPort);
      //the message takes an address as an required argument
      OSCMessage msg("/ST");
      //add data of any type to the end of the message with 'add'";
      msg.add("A3").add(valA_IN3);
      msg.send(Udp);
      Udp.endPacket();
      msg.empty();
      prevValA_IN3 = valA_IN3;
    }
    if (UdpConfig.a4 == 1 && valA_IN4 != prevValA_IN4){
      Udp.beginPacket(tIP, tPort);
      //the message takes an address as an required argument
      OSCMessage msg("/ST");
      //add data of any type to the end of the message with 'add'";
      msg.add("A4").add(valA_IN4);
      msg.send(Udp);
      Udp.endPacket();
      msg.empty();
      prevValA_IN4 = valA_IN4;
    }
  }
}