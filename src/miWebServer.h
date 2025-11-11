#ifndef MIWEBSERVER_H
#define MIWEBSERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

bool syncTimeWithNTP();
void wifiConnect();
bool isTimeSet();

#endif // MIWEBSERVER_H