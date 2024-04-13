// include libraries
#include <Arduino.h>
#include <WiFi.h>
#include <OSCMessage.h>
#include <ESPAsyncWebServer.h>
#include <BluetoothSerial.h>
#include <SPIFFS.h>

// Create an instance of AsyncWebServer
AsyncWebServer oscTerminalServer(80);

// UDP instance to send and receive packets over UDP
WiFiUDP Udp;

// Struct to store UDP and input settings
struct UserInput {
  // Target IP of UDP packages
  unsigned int tip1 = 192;
  unsigned int tip2 = 168;
  unsigned int tip3 = 1;
  unsigned int tip4 = 100;
  // Outgoing port of UDP packages
  unsigned int outPort = 1234;
  // Outgoing port of UDP packages
  unsigned int tPort = 8888;
  // Booleans for analog input pins
  bool a1 = 0;
  bool a2 = 0;
  bool a3 = 0;
  bool a4 = 0;
  // Boolean to check whether setup was completed
  bool setup = 0;
};
// Create instance of UserInput
UserInput UdpConfig;

UserInput oscTerminal(){
  // Start server for OSC Terminal
  oscTerminalServer.begin();
  // Terminal page
  oscTerminalServer.on("/MBytePC230.woff", HTTP_GET, [](AsyncWebServerRequest *request) { 
    request->send(SPIFFS, "/MBytePC230.woff", "application/x-font-woff");
  });

  oscTerminalServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/osc_terminal.html");
  });

  oscTerminalServer.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css","text/css");
  });

  // Pass user's network credentials to tCreds
  oscTerminalServer.on("/changesettings", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("tip1", true) && request->hasParam("tip2", true)) {
      if (request->hasParam("tip3", true) && request->hasParam("tip4", true))
        UdpConfig.tip1 = request->getParam("tip1", true)->value().toInt();
        UdpConfig.tip2 = request->getParam("tip2", true)->value().toInt();
        UdpConfig.tip3 = request->getParam("tip3", true)->value().toInt();
        UdpConfig.tip4 = request->getParam("tip4", true)->value().toInt();
    }
    if (request->hasParam("tPort", true) && request->hasParam("outPort", true)) {
      UdpConfig.tPort = request->getParam("tPort", true)->value().toInt();
      UdpConfig.outPort = request->getParam("outPort", true)->value().toInt();
    }
    if (request->hasParam("a1", true)) {
      UdpConfig.a1 = 1;
      Serial.println("A1 enabled");
    }
    if (request->hasParam("a2", true)) {
      UdpConfig.a2 = 1;
      Serial.println("A2 enabled");
    }
    if (request->hasParam("a3", true)) {
      UdpConfig.a3 = 1;
      Serial.println("A3 enabled");
    }
    if (request->hasParam("a4", true)) {
      UdpConfig.a4 = 1;
      Serial.println("A4 enabled");
    }
    UdpConfig.setup = 1;
    // Redirect to success page
    request->redirect("/osc_running");
  });

  oscTerminalServer.on("/osc_running", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/osc_running.html");
  });

  oscTerminalServer.on("/reset", HTTP_POST, [](AsyncWebServerRequest *request) {
    UdpConfig.setup = 0;
    UdpConfig.a1 = 0;
    UdpConfig.a2 = 0;
    UdpConfig.a3 = 0;
    UdpConfig.a4 = 0;
    Udp.stop();
    // Redirect to terminal page
    request->redirect("/osc_terminal");
  });

  oscTerminalServer.on("/osc_terminal", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/osc_terminal.html");
  });
  return UdpConfig;
}