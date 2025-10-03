/*
  Uso el MAX7219 en la plaqueta del display.
  Este es un test para ver como hacemos para mostrar segun el circuito utilizado.
  Javier.
  septiembre 2025
*/
#include <MD_MAX72xx.h>
#include <SPI.h>

/*
  Display de 7x5 segun el circuito usando GENERIC_HW.
  setPoint(row, col)

  rows 1 a 7 => T4  x x x x x x x x  (row 0 no existe)
                A   0 0 0 0 0 x x x
                B   0 0 0 0 0 x x x
                C   0 0 0 0 0 x x x
                D   0 0 0 0 0 x x x
                E   0 0 0 0 0 x x x
                F   0 0 0 0 0 x x x
                G   0 0 0 0 0 x x x
                    | | | | | | | |
                    D D D D D T T T
                    0 1 2 3 4 3 2 1
                    ^
                    |
                    +-- cols 0 a 4  (cols 5 a 7 no existen)
*/

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
  Serial.print("MAX7219 Test");
  pinMode(0, INPUT);
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
  // point J1-J4
  // mx.setPoint(5, 0, true);
  mx.setPoint(6, 0, true);
  // mx.setPoint(7, 0, true);
  
  delay(500);
  mx.clear();//Point(7, 0, false);
}
