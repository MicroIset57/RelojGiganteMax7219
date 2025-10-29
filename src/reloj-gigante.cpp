/*
  Uso el MAX7219 en la plaqueta del display.
  Este es un test para ver como hacemos para mostrar segun el circuito utilizado.
  Javier.
  septiembre 2025
*/
#include "MD_MAX72xx/MD_MAX72xx.h"
#include <time.h>
#include "Leds.h"
#include "miWebServer.h"
#include "config.h"

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

// Las configuraciones se han movido a config.h

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
  setNumero(DIG0, hr / 10);
  setNumero(DIG1, hr % 10);
}

void setMinuto(int min)
{
  setNumero(DIG2, min / 10);
  setNumero(DIG3, min % 10);
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
  AllLeds(CRGB::Black);

  // enciendo solo Lunes a Viernes
  //   lunes     : 44 al 50
  //   martes    : 38 al 43
  //   miercoles : 31 al 36 (el 37 no usar!)
  //   jueves    : 24 al 30
  //   viernes   : 18 al 23 (17 no usar!)
  if (dia == 1)
  {
    AllLeds(CRGB::Cyan, 44, 50);
  }
  else if (dia == 2)
  {
    AllLeds(CRGB::Cyan, 38, 43);
  }
  else if (dia == 3)
  {
    AllLeds(CRGB::Cyan, 31, 36);
  }
  else if (dia == 4)
  {
    AllLeds(CRGB::Cyan, 24, 30);
  }
  else if (dia == 5)
  {
    AllLeds(CRGB::Cyan, 18, 23);
  }
  // sabados y domingos no enciendo nada
}

void setup()
{
  Serial.begin(115200);
  mx.begin();
  resetMax();
  Serial.println("RESET!\n**Reloj Gigante ISET 57**\ninicializando...");

  // Inicializa los WS2812B
  LedsInit();
  // FastLED.setBrightness(30);
  LedsKitt(CRGB::Red, 0, 16);

  // Iniciar servidor y el wifi
  initWebServer();
  Serial.println("Servidor web iniciado");

  // simulo encendido de dias L-M-M-J-V
  for (int i = 1; i <= 5; i++)
  {
    delay(150);
    setDias(i);
  }
  delay(150);
  AllLeds(CRGB::Black);
  delay(500);

  Serial.println("setup listo.");
}

void loop()
{
  static bool sabadomingo = false;
  static bool clockError = false;
  static CHSV randomColor(135, 255, 240);

  // FastLED.setBrightness(30);

  // intenta reconectarse cada vez que no este conectado.
  if (WiFi.status() != WL_CONNECTED)
  {
    wifiConnect(); // intenta durante 10 segs.
    AllLeds(CRGB::Red);
    delay(2000);
    AllLeds(CRGB::Black);
    return;
  }

  // la primera vez y cada 1 hora se sincroniza con el reloj de internet.
  if (!syncTimeWithNTP())
  {
    AllLeds(CRGB::Yellow); // fallo la ponida de hora!
    delay(2000);
    AllLeds(CRGB::Black);
    return;
  }

  // Actualiza los leds (no son bloqueantes)
  EVERY_N_MILLIS(80)
  {
    if (sabadomingo)
      updateRainbow(); // colores en todos los leds (dias de semana y ISET 57)
    else
      NextLed(0, 16, randomColor); // muestra en el ISET57 solo random por dia...
  }

  EVERY_N_MILLIS(500)
  {
    setPuntos(); // cada 500 ms prende/apaga.

    if (isTimeSet())
    {
      // obtengo hora actual
      time_t now;
      struct tm timeinfo;
      time(&now);
      localtime_r(&now, &timeinfo);

      // solo muestro cada minuto
      static int last_min = -1;
      if (timeinfo.tm_min != last_min)
      {
        last_min = timeinfo.tm_min;
        Serial.print("Hora: ");
        Serial.print(timeinfo.tm_hour);
        Serial.print(":");
        Serial.println(timeinfo.tm_min);

        setHora(timeinfo.tm_hour);
        setMinuto(timeinfo.tm_min);

        // Solo mostrar el arcoiris sabados y domingos:
        sabadomingo = (timeinfo.tm_wday == 0 || timeinfo.tm_wday == 6);
        if (!sabadomingo)
        {
          setDias(timeinfo.tm_wday); // enciende el dia de la semana
          static int lastWday = -1;
          if (timeinfo.tm_wday != lastWday)
          {
            lastWday = timeinfo.tm_wday;
            randomColor = CHSV(random(0, 255), 255, 240);
          }
        }
      }
    }
  }
}
