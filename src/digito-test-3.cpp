/*
  Uso el MAX7219 en la plaqueta del display.
  Este es un test para ver como hacemos para mostrar segun el circuito utilizado.
  Javier.
  septiembre 2025
*/
#include <MD_MAX72xx.h>
#include <SPI.h>
// Control de tiras WS2812B
#include <FastLED.h>

#define NUM_LEDS 7
#define LED_PIN 15 // GPIO15
CRGB leds[NUM_LEDS];

// Parámetros del arcoiris
const uint32_t RAINBOW_INTERVAL_MS = 80; // velocidad del desplazamiento

/*
  4 Displays de 7 segmentos

                A              A        A        A              A
            +-------+      +-------+         +-------+      +-------+
            |       |      |       |         |       |      |       |
           F|   G   |B    F|   G   |B   O    F|   G   |B    F|   G   |B
            +-------+      +-------+    |    +-------+      +-------+
            |       |      |       |    O    |       |      |       |
           E|   D   |C    E|   D   |C   |   E|   D   |C    E|   D   |C
            +-------+      +-------+    |    +-------+      +-------+
                |              |        |        |              |
               DIG0           DIG1     DIG4     DIG2           DIG3

  setPoint(row, col)
  cols = A,B,C,D,E,F,G  (7 cols)
  rows = DIG0,DIG1,DIG2,DIG3,DIG4 (5 rows)
*/

// para PAROLA_HW
#define SEGA 1
#define SEGB 2
#define SEGC 3
#define SEGD 4
#define SEGE 5
#define SEGF 6
#define SEGG 7
#define DIG0 0
#define DIG1 1
#define DIG2 2
#define DIG3 3
#define DIG4 4
// donde comienza el led del ISET 57 y donde termina:

#define ISET57_BEGIN 5
#define ISET57_END 6

// https://arduinoplusplus.wordpress.com/2017/04/14/parola-a-to-z-adapting-for-different-hardware/
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW // PAROLA_HW // GENERIC_HW
#define MAX_DEVICES 1
#define CLK_PIN 18  // or SCK
#define DATA_PIN 23 // or MOSI
#define CS_PIN 5    // or SS

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES); // SPI hardware interface

void resetMax(void)
{
  mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY / 3);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  mx.clear();
}

// Enciende un digito con el numero indicado.
void setNumero(int digito, int num)
{
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.setPoint(digito, SEGA, true);
  mx.setPoint(digito, SEGB, true);
  mx.setPoint(digito, SEGC, true);
  mx.setPoint(digito, SEGD, true);
  mx.setPoint(digito, SEGE, true);
  mx.setPoint(digito, SEGF, true);
  mx.setPoint(digito, SEGG, true);

  if (num == 0)
  {
    mx.setPoint(digito, SEGG, false);
  }
  else if (num == 1)
  {
    mx.setPoint(digito, SEGA, false);
    mx.setPoint(digito, SEGD, false);
    mx.setPoint(digito, SEGE, false);
    mx.setPoint(digito, SEGF, false);
    mx.setPoint(digito, SEGG, false);
  }
  else if (num == 2)
  {
    mx.setPoint(digito, SEGC, false);
    mx.setPoint(digito, SEGF, false);
  }
  else if (num == 3)
  {
    mx.setPoint(digito, SEGE, false);
    mx.setPoint(digito, SEGF, false);
  }
  else if (num == 4)
  {
    mx.setPoint(digito, SEGA, false);
    mx.setPoint(digito, SEGD, false);
    mx.setPoint(digito, SEGE, false);
  }
  else if (num == 5)
  {
    mx.setPoint(digito, SEGB, false);
    mx.setPoint(digito, SEGE, false);
  }
  else if (num == 6)
  {
    mx.setPoint(digito, SEGB, false);
  }
  else if (num == 7)
  {
    mx.setPoint(digito, SEGD, false);
    mx.setPoint(digito, SEGE, false);
    mx.setPoint(digito, SEGF, false);
    mx.setPoint(digito, SEGG, false);
  }
  else if (num == 8)
  {
    // ya esta encendido todo
  }
  else if (num == 9)
  {
    mx.setPoint(digito, SEGE, false);
  }
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void setHora(int hr)
{
  int dig1 = hr / 10;
  int dig0 = hr % 10;
  setNumero(DIG0, dig0);
  setNumero(DIG1, dig1);
}

void setMinuto(int min)
{
  int dig1 = min / 10;
  int dig0 = min % 10;
  setNumero(DIG2, dig0);
  setNumero(DIG3, dig1);
}

// leds de los puntos entre hora y minuto: [DIG4,A]
void setPuntos()
{
  static bool estado = true;
  estado = !estado;
  mx.setPoint(DIG4, SEGA, estado);
}

// leds[0] = domingo ... leds[6] = sábado
// yo voy a usar dia 1 (lunes) a 5 (viernes)
void setDias(int dia)
{
  // primero apago todos los leds:
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Black;
  }

  // enciendo solo ese dia.
  if (dia == 1) // lunes
  {
    leds[0] = CRGB::Red;
  }
  else if (dia == 2) // martes
  {
    leds[1] = CRGB::Red;
  }
  else if (dia == 3) // miércoles
  {
    leds[2] = CRGB::Red;
  }
  else if (dia == 4) // jueves
  {
    leds[3] = CRGB::Red;
  }
  else if (dia == 5) // viernes
  {
    leds[4] = CRGB::Red;
  }
  else
  {
    // sábado y domingo se queda boludeando....
    // ARCOIRIS:
  }
  FastLED.show();
}

void setup()
{
  Serial.begin(115200);
  mx.begin();
  resetMax();
  Serial.println("Reloj Gigante ISET 57");
  // Inicializa los WS2812B
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
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

void updateIset57()
{
  static int c = -1;
  c++;

  if (c == 0)
  {
    fill_gradient_RGB(leds, ISET57_BEGIN, CRGB::Blue, ISET57_END, CRGB::Blue4);
  }
  else if (c < 100)
  {
    fadeToBlackBy(leds, ISET57_END - ISET57_BEGIN + 1, 10);
  }
  else
  {
    c = 0;
  }
}

void loop()
{
  static bool showRainbow = false;
  // Actualiza arcoiris (si está activado). updateRainbow() es no bloqueante.
  EVERY_N_MILLIS(RAINBOW_INTERVAL_MS)
  {
    if (showRainbow)
      updateRainbow();
    else
      updateIset57();
  }

  EVERY_N_MILLIS(500)
  {
    setPuntos(); // cada 500 ms prende/apaga.

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    setHora(timeinfo.tm_hour);
    setMinuto(timeinfo.tm_min);

    // Solo mostrar el arcoiris sabados y domingos:
    showRainbow = (timeinfo.tm_wday == 0 || timeinfo.tm_wday == 6);
    if (!showRainbow)
    {
      setDias(timeinfo.tm_wday); // enciende el dia de la semana
    }
  }
}
