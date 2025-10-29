/*
  Uso el MAX7219 en la plaqueta del display.
  Este es un test para ver como hacemos para mostrar segun el circuito utilizado.
  Javier.
  septiembre 2025
*/
#include "MD_MAX72xx/MD_MAX72xx.h"
#include <WiFi.h>
#include <time.h>
#include "Leds.h"

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

// --- WiFi / NTP config (modifica aquí con tus credenciales y zona horaria) ---
#define WIFI_SSID "ISET57CLARO"
#define WIFI_PASS "GONZALO1981"

// Offset en segundos respecto a UTC (ejemplo Argentina: -3h -> -10800)
#define GMT_OFFSET_SEC (-3 * 3600)
#define DAYLIGHT_OFFSET_SEC 0
#define NTP_SERVER_1 "pool.ntp.org"
#define NTP_SERVER_2 "time.nist.gov"

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

// ---------- WiFi / NTP ----------
void connectToWiFi(unsigned long timeoutMs = 15000)
{
  Serial.printf("Conectando a WiFi '%s'...\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int x = 0;
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs)
  {
    FastLED.showColor(((++x % 2) == 0) ? CRGB::Magenta3 : CRGB::Black);
    Serial.print('.');
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println();
    Serial.print("WiFi conectado, IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println();
    Serial.println("No se pudo conectar a WiFi (timeout)");
    FastLED.showColor(CRGB::Red);
    delay(1000);
  }
}

void syncTimeWithNTP(unsigned long timeoutMs = 15000)
{
  Serial.println("Iniciando sync NTP...");
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER_1, NTP_SERVER_2);
  unsigned long start = millis();
  time_t now;
  int x = 0;
  while ((millis() - start) < timeoutMs)
  {
    FastLED.showColor(((++x % 2) == 0) ? CRGB::Cyan3 : CRGB::Black);
    now = time(nullptr);
    if (now > 100000)
    {
      struct tm timeinfo;
      localtime_r(&now, &timeinfo);
      char buf[64];
      strftime(buf, sizeof(buf), "%c", &timeinfo);
      Serial.print("Hora sincronizada: ");
      Serial.println(buf);
      return;
    }
    delay(200);
  }
  Serial.println("No se pudo sincronizar la hora (timeout)");
  FastLED.showColor(CRGB::Red);
  delay(1000);
}

bool isTimeSet()
{
  time_t now;
  time(&now);
  return (now > 1759968000); // verifico si es mayor a 9/10/2025
}

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

  // enciendo solo ese dia.
  if (dia == 1) // lunes
  {
    SetLed(0, CRGB::Red);
  }
  else if (dia == 2) // martes
  {
    SetLed(1, CRGB::Red);
  }
  else if (dia == 3) // miércoles
  {
    SetLed(2, CRGB::Red);
  }
  else if (dia == 4) // jueves
  {
    SetLed(3, CRGB::Red);
  }
  else if (dia == 5) // viernes
  {
    SetLed(4, CRGB::Red);
  }
  else
  {
    // sábado y domingo no hace nada...
  }
}

void setup()
{
  Serial.begin(115200);
  mx.begin();
  resetMax();
  Serial.println("Reloj Gigante ISET 57");

  // Inicializa los WS2812B
  LedsInit();
  // FastLED.setBrightness(30);
  LedsKitt(CRGB::Red);

  connectToWiFi();   // Agregar conexión WiFi
  syncTimeWithNTP(); // Sincronización NTP
}

void loop()
{
  static bool sabadomingo = false;
  static bool clockError = false;

  // Actualiza los leds (no son bloqueantes)
  EVERY_N_MILLIS(80)
  {
    if (clockError)
      LedFlash(CRGB::Red4); // muestro error
    else if (sabadomingo)
      updateRainbow(); // colores en todos los leds, dias de semana y ISET 57.
    else
      NextLed(ISET57_BEGIN, ISET57_END, CRGB::Blue1);
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
    sabadomingo = (timeinfo.tm_wday == 0 || timeinfo.tm_wday == 6);
    if (!sabadomingo)
    {
      setDias(timeinfo.tm_wday); // enciende el dia de la semana
    }

    // verifico internet:
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Se perdió la conexión WiFi. Reintentando...");
      connectToWiFi();   // conexión WiFi
      syncTimeWithNTP(); // Sincronización NTP
    }
    else if (clockError)
    {
      Serial.println("Se perdió la hora. Reintentando...");
      WiFi.disconnect();
    }
    clockError = !isTimeSet(); // si no esta en hora todos los leds quedaran rojos!!!
  }
}
