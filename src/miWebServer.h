#ifndef MIWEBSERVER_H
#define MIWEBSERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <WiFiMulti.h>
#include "config.h"

extern WebServer server;

void initWebServer();
void handleRoot();
void handleTimeUpdate();
void handleNotFound();
bool syncTimeWithNTP();
void wifiConnect();
bool isTimeSet();

#endif // MIWEBSERVER_H