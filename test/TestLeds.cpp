/*
    TEST DE LEDS.

  Javier.
  septiembre 2025
*/

#include "Leds.h"

void setup()
{
    Serial.begin(115200);
    delay(555);
    Serial.println("TEST DE RUTINA DE LEDS");
    LedsInit();
    // leo boton para probar led a led:
    pinMode(0, INPUT_PULLUP);

    Serial.println("PULSA EL BOTON BOOT PARA PROBAR LED A LED");
}

void TestLeds()
{
    // NextLed:
    NextLed(0, NUM_LEDS, CRGB::Blue, false);
}

void loop()
{
    // leo pulsador y en cada flanco muevo un led:
    static bool lastState = HIGH;
    bool currentState = digitalRead(0);
    if (lastState == HIGH && currentState == LOW)
    {
        TestLeds();
        delay(222); // anti-rebote
    }
    lastState = currentState;
}
