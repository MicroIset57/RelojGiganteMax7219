#include "Leds.h"

CRGB leds[NUM_LEDS];

void LedsInit()
{
    static bool inicializado = false;
    if (!inicializado)
    {
        FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
        // apago todo.
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        inicializado = true;
    }
}

void ShiftLed(CRGB col, uint8_t brillo /*255 default*/)
{
    static int led = 0;
    leds[led] = CRGB::Black;
    if (++led == NUM_LEDS)
        led = 0;

    leds[led] = col;
    if (brillo < 255)
        leds[led].nscale8(brillo);

    FastLED.show();
}

void NextLed(int desde, int hasta, CRGB col, bool dejarCola /*= true*/)
{
    static int led = desde;
    static bool sentidoDerecho = true;
    if (!dejarCola)
        leds[led] = CRGB::Black;

    // Serial.print("LED ");
    // Serial.print(led);
    // Serial.print(" APAGADO ");

    if (sentidoDerecho)
    {
        if (++led > hasta)
        {
            sentidoDerecho = false;
            led--; // me pase 1, vuelvo.
        }
    }
    else
    {
        if (--led < desde)
        {
            sentidoDerecho = true;
            led++; // me pase 1, vuelvo.
        }
    }

    if (dejarCola)
        fadeToBlackBy(leds + desde, hasta - desde, 120);

    leds[led] = col;

    // Serial.print(" -  LED ");
    // Serial.print(led);
    // Serial.println(" ENCENDIDO ");

    FastLED.show();
}

void ToggleLeds(CRGB col1, uint8_t brillo1, CRGB col2, uint8_t brillo2)
{
    static int c = 0;
    if (++c % 2 == 0)
        AllLeds(col1, brillo1);
    else
        AllLeds(col2, brillo2);
}

void AllLeds(CRGB col, uint8_t brillo)
{
    fill_solid(leds, NUM_LEDS, col);
    if (brillo < 255)
        fadeLightBy(leds, NUM_LEDS, brillo);
    FastLED.show();
}

void AllLeds(CRGB col, int ini, int fin)
{
    for (int i = ini; i <= fin; i++)
        leds[i] = col;
    FastLED.show();
}

void LedFlash(CRGB col)
{
    FastLED.setBrightness(255);
    fill_solid(leds, NUM_LEDS, col);
    FastLED.show();
    delay(50);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
}

void Cocoon(CRGB col)
{
    static uint8_t fade = 0;
    static int direccion = 2;
    EVERY_N_MILLIS(25)
    {
        AllLeds(col, fade += direccion);
        if (fade < 2)
            direccion = 2;
        if (fade > 253)
            direccion = -2;
    }
}

// "Knight Industries Two Thousand"
void LedsKitt(CRGB color, int ini, int end)
{
    AllLeds(CRGB::Black);
    for (int i = ini; i < end + 2; ++i)
    {
        if (i < end)
            leds[i] = color;
        fadeToBlackBy(leds, end, 70);
        FastLED.show();
        FastLED.delay(33);
    }
    for (int i = ini; i < end * 2; ++i)
    {
        if (i < end)
            leds[end - 1 - i] = color;
        fadeToBlackBy(leds, end, 70);
        FastLED.show();
        FastLED.delay(33);
    }
}

// Actualiza el arreglo de LEDs con un arcoiris que se desplaza hacia la derecha.
void updateRainbow()
{
    static uint8_t startHue = 0;
    // Usamos fill_rainbow para rellenar desde startHue.
    // Luego desplazamos hacia la derecha una posición para dar la sensación de movimiento.
    startHue += 4; // velocidad de cambio de color base
    fill_rainbow(leds, NUM_LEDS, startHue, 8);
    // Para que parezca que se mueve hacia la derecha, rotamos el array una posición a la right
    CRGB lastColor = leds[NUM_LEDS - 1];
    for (int i = NUM_LEDS - 1; i > 0; --i)
        leds[i] = leds[i - 1];
    leds[0] = lastColor;
    FastLED.show();
}

void SetLed(int ledNum, CRGB col)
{
    if (ledNum < 0 || ledNum >= NUM_LEDS)
        return;
    leds[ledNum] = col;
    FastLED.show();
}