/*
  Uso el MAX7219 en la plaqueta del display.
  Este es un test para ver como hacemos para mostrar segun el circuito utilizado.
  Javier.
  septiembre 2025
*/
#include <MD_MAX72xx.h>
#include <SPI.h>

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
  rows = A,B,C,D,E,F,G  (7 rows)
  cols = DIG0,DIG1,DIG2,DIG3,DIG4 (5 cols)
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

// https://arduinoplusplus.wordpress.com/2017/04/14/parola-a-to-z-adapting-for-different-hardware/
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW // PAROLA_HW // GENERIC_HW
#define MAX_DEVICES 1
#define CLK_PIN 18  // or SCK
#define DATA_PIN 23 // or MOSI
#define CS_PIN 5    // or SS

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES); // SPI hardware interface
// MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES); // Arbitrary pins

void resetMax(void)
{
  mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY / 3);
  // mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  mx.clear();
}

void setup()
{
  Serial.begin(115200);
  mx.begin();
  resetMax();
  Serial.println("Reloj Gigante ISET 57");
}

void loop(void)
{
  static int c_ant = 0, r_ant = 0;
  for (size_t c = 1; c <= 7; c++)
  {
    for (size_t r = 0; r <= 4; r++)
    {
      /*while (digitalRead(0) == 1)
        ;
      delay(200);*/

      // mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
      mx.setPoint(r_ant, c_ant, false);
      mx.setPoint(r, c, true);
      c_ant = c;
      r_ant = r;
      // mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
      delay(100);
    }
  }
  mx.setPoint(r_ant, c_ant, false);
  // point [DIG4,A]
  // mx.setPoint(5, 0, true);
  mx.setPoint(SEGA, DIG4, true);
  // mx.setPoint(7, 0, true);
  
  delay(500);
  mx.clear();//Point(7, 0, false);
}
