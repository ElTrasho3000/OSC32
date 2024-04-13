// include libraries
#include <Arduino.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <SPIFFS.h>


void startStationMode(){
  // Create IPAddress variables
  IPAddress staticIP;
  IPAddress router;
  IPAddress snet;
  // Assign espIP obtained from user to IPAddress staticIP
  File espIP = SPIFFS.open("/espIP.txt", "r");
  if (!espIP){
    Serial.println("Failed to open/create file.");
  }
  if (espIP) { 
    staticIP.fromString(espIP.readString());
    Serial.println(staticIP);
  }
  // Assign routerIP obtained from user to IPAddress router
  File routerIP = SPIFFS.open("/routerIP.txt", "r");
  if (!routerIP){
    Serial.println("Failed to open/create file.");
  }
  if (routerIP) { 
    router.fromString(routerIP.readString());
    Serial.println(router);
  }
  // Assign snetIP obtained from user to IPAddress snet 
  File snetIP = SPIFFS.open("/snetIP.txt", "r");
  if (!snetIP){
    Serial.println("Failed to open/create file.");
  }
  if (snetIP) { 
    snet.fromString(snetIP.readString());
    Serial.println(snet);
  }
  // Configures static IP address
  if (!WiFi.config(staticIP, router, snet)) {
    Serial.println("STA failed to configure");
  }
  // Connect to user's network using 
  // the obtained credentials
  File ssid = SPIFFS.open("/ssid.txt", "r");
  if (!ssid){
    Serial.println("Failed to open/create file.");
  }
  File password = SPIFFS.open("/password.txt", "r");
  if (!password){
    Serial.println("Failed to open/create file.");
  }
  if (ssid && password){
    WiFi.begin(ssid.readString(), password.readString());
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
  Serial.println("Connected to WiFi");
  }
}