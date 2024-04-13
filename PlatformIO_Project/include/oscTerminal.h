#ifndef OSC_TERMINAL_H
#define OSC_TERMINAL_H

#include <Arduino.h>
#include <WiFi.h>
#include <OSCMessage.h>
#include <ESPAsyncWebServer.h>
#include <BluetoothSerial.h>
#include <SPIFFS.h>

extern AsyncWebServer oscTerminalServer;
extern WiFiUDP Udp;

struct UserInput {
  unsigned int tip1;
  unsigned int tip2;
  unsigned int tip3;
  unsigned int tip4;
  unsigned int outPort;
  unsigned int tPort;
  bool a1;
  bool a2;
  bool a3;
  bool a4;
  bool setup;
};

extern UserInput UdpConfig;

UserInput oscTerminal();

#endif // OSC_TERMINAL_H