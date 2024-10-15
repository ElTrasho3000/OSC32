// Include libraries
#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// link other files
#include "deviceID.h"

// Create an instance of AsyncWebServer
AsyncWebServer setupServer(80);

bool setupRunning = false;

bool wifiSetup(){
    // Set bool indicating running setup to true
    setupRunning = true;
    // AccessPoint Name with DEVICE_ID
    String apName = "OSC32-Network-Setup-" + DEVICE_ID;
    // Start Access Point
    WiFi.softAP(apName, "password123");
    // Start webserver for OSC Setup Terminal
    setupServer.begin();
    // Return false while setup is running
    // Load font  
    setupServer.on("/MBytePC230.woff", HTTP_GET, [](AsyncWebServerRequest *request) { 
        request->send(SPIFFS, "/MBytePC230.woff", "application/x-font-woff");
    });
    // Load main page
    setupServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/network_setup.html");
    });
    // Load css-file
    setupServer.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/style.css","text/css");
    });
    // Pass users network credentials to SPIFFS
    setupServer.on("/setcreds", HTTP_POST, [](AsyncWebServerRequest *request) {
        // Store SSID and password in files
        if (request->hasParam("ssid", true) && request->hasParam("pwd", true)) {
            // Store values in variables
            String ssidVar = request->getParam("ssid", true)->value();
            String pwd = request->getParam("pwd", true)->value();
            // If file exists, open and delete content.
            // If file doesn't exist, create and open file.
            File ssid = SPIFFS.open("/ssid.txt", "w+");
            // Print warning to serial if opening file fails.
            if (!ssid){
                Serial.println("Failed to open file!");
            }
            // Write SSID to file, if file is available for writing.
            // Then close file.
            if (ssid){
                ssid.print(ssidVar);
                ssid.close();
                Serial.println("SSID set to: " + ssidVar);
            }
            // If file exists, open and delete content.
            // If file doesn't exist, create and open file.
            File password = SPIFFS.open("/password.txt","w+");
            // Print warning to serial if opening file fails.
            if (!password){
                Serial.println("Failed to open file!");
            }    
            // Write password to file, if file is available for writing.
            // Then close file.
            if (password){
                password.print(pwd);
                password.close();
                Serial.println("Password set to: " + pwd);
            }     
            // Store static IP of esp32.
            if (request->hasParam("eip1", true) && request->hasParam("eip2", true)) {
                if (request->hasParam("eip3", true) && request->hasParam("eip4", true)) { 
                    // Store values in variables 
                    String eip1 = request->getParam("eip1", true)->value();
                    String eip2 = request->getParam("eip2", true)->value();
                    String eip3 = request->getParam("eip3", true)->value();
                    String eip4 = request->getParam("eip4", true)->value();
                    Serial.println("Values stored");
                    // Construct string in ip format
                    String eip = eip1 + "." + eip2 + "." + eip3 + "." + eip4;
                    Serial.println("IP created");
                    // Open/create file
                    File espIP = SPIFFS.open("/espIP.txt", "w+");
                    // Return error if unable to open/create file
                    if (!espIP){
                        Serial.println("Failed to open/create file.");
                    }
                    Serial.println("File opened");
                    // Write IPAddress to file, if available for writing
                    if (espIP){
                        espIP.print(eip);
                        espIP.close();
                        Serial.println("ESP IP is: " + eip);
                    }
                }
            }
            // Store IP of gateway
            if (request->hasParam("rip1", true) && request->hasParam("rip2", true)) {
                if (request->hasParam("rip3", true) && request->hasParam("rip4", true)) {
                    // Store values in variables 
                    String rip1 = request->getParam("rip1", true)->value();
                    String rip2 = request->getParam("rip2", true)->value();
                    String rip3 = request->getParam("rip3", true)->value();
                    String rip4 = request->getParam("rip4", true)->value();
                    // Create IPAddress from variables
                    String rip = rip1 + "." + rip2 + "." + rip3 + "." + rip4;
                    // Open/create file
                    File routerIP = SPIFFS.open("/routerIP.txt", "w+");
                    // Return error if unable to open/create file
                    if (!routerIP){
                        Serial.println("Failed to open/create file.");
                    }
                    // Write IPAddress to file, if available for writing
                    if (routerIP){
                        routerIP.print(rip);
                        routerIP.close();
                        Serial.println("Router IP is: "+ rip);
                    }
                }
            }
            // Store IP of subnet
            if (request->hasParam("snet1", true) && request->hasParam("snet2", true)) {
                if (request->hasParam("snet3", true) && request->hasParam("snet4", true)) {
                    String snet1 = request->getParam("snet1", true)->value();
                    String snet2 = request->getParam("snet2", true)->value();
                    String snet3 = request->getParam("snet3", true)->value();
                    String snet4 = request->getParam("snet4", true)->value();
                    // Create IPAddress from variables
                    String snet = snet1 + "." + snet2 + "." + snet3 + "." + snet4;
                    // Open/create file
                    File snetIP = SPIFFS.open("/snetIP.txt", "w+");
                    // Return error if unable to open/create file
                    if (!snetIP){
                        Serial.println("Failed to open/create file.");
                    }
                    // Write IPAddress to file, if available for writing
                    if (snetIP){
                        snetIP.print(snet);
                        snetIP.close();
                        Serial.println("Subnet Mask is: "+ snet);
                    }
                }
            }
            // Redirect to success page
            request->redirect("/success");
        }
        // Error page
        else {
        request->send(400, "text/html", "<h2>Invalid request</h2>");
        }
    });
    // Load success page
    setupServer.on("/success", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/success.html");
        delay(2000);
        // Stop server and end softAP
        setupServer.end();
        Serial.println("Server stopped");
        // Disconnect from Access Point
        WiFi.softAPdisconnect(true);
        // Exit setup
        setupRunning = false;
    });
  
    return setupRunning;
}