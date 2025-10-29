#pragma once
#include <FastLED.h>

#define NUM_LEDS ((5 * 7) + 17) // 5 dias de semana + ISET57 = 52 leds
#define LED_PIN 15
/*
    lunes     : 44 al 50
    martes    : 38 al 43
    miercoles : 31 al 36 (el 37 no usar!)
    jueves    : 24 al 30
    viernes   : 18 al 23 (17 no usar!)
    ISET 57   : 0 al 16 (el 17 no usar, se encienden 2 juntos)
*/

void LedsInit();
void NextLed(int desde, int hasta, CRGB col, bool dejarCola = true);
void ShiftLed(CRGB col, uint8_t brillo = 255);
void AllLeds(CRGB col, uint8_t brillo = 255);
void AllLeds(CRGB col, int ini, int fin);
void LedFlash(CRGB col);
void LedsKitt(CRGB color, int, int);
void ToggleLeds(CRGB col1, uint8_t brillo1, CRGB col2, uint8_t brillo2);
void updateRainbow();
void SetLed(int ledNum, CRGB col);
void Cocoon(CRGB col);
