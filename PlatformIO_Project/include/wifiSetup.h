#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer setupServer;

extern bool setupRunning;

bool wifiSetup();

#endif // WIFI_SETUP_H
