#ifndef MIWEBSERVER_H
#define MIWEBSERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "config.h"

// Funciones originales
bool syncTimeWithNTP();
void wifiConnect();
bool isTimeSet();

// Nuevas funciones para gestión de credenciales WiFi
void saveWiFiCredentials(const char* ssid, const char* password);
bool loadWiFiCredentials(String &ssid, String &password);
void clearWiFiCredentials();
bool hasStoredCredentials();

// Funciones para modo Access Point y servidor de configuración
void startConfigPortal();
void stopConfigPortal();
bool isConfigPortalActive();

#endif // MIWEBSERVER_H
