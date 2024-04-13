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

// GPIO 23 as a switch to toggle setup-mode.
const unsigned int SETUP_SWITCH = 27;

// GPIO pin
const unsigned int aPin1 = 34;
const unsigned int aPin2 = 35;
const unsigned int aPin3 = 32;
const unsigned int aPin4 = 33;

// Variables to store and compare input values
unsigned int prevValaPin1 = 0;
unsigned int prevValaPin2 = 0;
unsigned int prevValaPin3 = 0;
unsigned int prevValaPin4 = 0;

void setup() {
  // Use internal pullup-resistor to set high.
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
        delay(1000);
      };
      // Should we automatically put esp32 into deep sleep mode here?
  }
  // Connect to user's network
  startStationMode();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  // Initiate OSC-Terminal
  oscTerminal();
}

void loop(){
  while (UdpConfig.setup == 0){
    delay(1000);
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
    unsigned int valaPin1 = map((analogRead(aPin1)/100), 0, 40, 0, 100);
    unsigned int valaPin2 = map((analogRead(aPin2)/100), 0, 40, 0, 100);
    unsigned int valaPin3 = map((analogRead(aPin3)/100), 0, 40, 0, 100);
    unsigned int valaPin4 = map((analogRead(aPin4)/100), 0, 40, 0, 100);
    // Wait until user configured UDP
    while (UdpConfig.a1 == 1 && valaPin1 != prevValaPin1){
      Udp.beginPacket(tIP, tPort);
      //the message takes an address as an required argument
      OSCMessage msg1("/ST");
      //add data of any type to the end of the message with 'add'";
      msg1.add("A1").add(valaPin1);
      msg1.send(Udp);
      Udp.endPacket();
      msg1.empty();
      prevValaPin1 = valaPin1;
    }
    if (UdpConfig.a2 == 1 && valaPin2 != prevValaPin2){
      Udp.beginPacket(tIP, tPort);
      //the message takes an address as an required argument
      OSCMessage msg2("/ST");
      //add data of any type to the end of the message with 'add'";
      msg2.add("A2").add(valaPin2);
      msg2.send(Udp);
      Udp.endPacket();
      msg2.empty();
      prevValaPin2 = valaPin2;
    }
    if (UdpConfig.a3 ==1 && valaPin3 != prevValaPin3){
      Udp.beginPacket(tIP, tPort);
      //the message takes an address as an required argument
      OSCMessage msg("/ST");
      //add data of any type to the end of the message with 'add'";
      msg.add("A3").add(valaPin3);
      msg.send(Udp);
      Udp.endPacket();
      msg.empty();
      prevValaPin3 = valaPin3;
    }
    if (UdpConfig.a4 == 1 && valaPin4 != prevValaPin4){
      Udp.beginPacket(tIP, tPort);
      //the message takes an address as an required argument
      OSCMessage msg("/ST");
      //add data of any type to the end of the message with 'add'";
      msg.add("A4").add(valaPin4);
      msg.send(Udp);
      Udp.endPacket();
      msg.empty();
      prevValaPin4 = valaPin4;
    }
  }
}