#include "MatrixCharlieplex.h"  // must be included

using namespace ArduinoMatrixCharlieplex;  // must use the designated namespace

// declare pins to control a (5 * 4) matrix Charlieplexed LED setup
uint8_t Pins[] = {8, 9, 10, 11, 12};

// declare a MatrixCharlieplex instance with
//  - Pin array
//  - number of pins
//  - common row method (available options are MXCHARLIE_CA[Common
//    Anode], MXCHARLIE_CC[Common Cathode], )
MatrixCharlieplex mch(Pins, 5, MXCHARLIE_CA);

int i = 0, j = 0, no = 0, count = 5;
unsigned long time;
int letter[][5] = {{0b0010, 0b0110, 0b0010, 0b0010, 0b0111},
                   {0b0110, 0b1001, 0b0010, 0b0100, 0b1111},
                   {0b0110, 0b1001, 0b0010, 0b1001, 0b0110},
                   {0b0010, 0b0110, 0b1010, 0b1111, 0b0010},
                   {0b1111, 0b1110, 0b0001, 0b1001, 0b0110}};
boolean busy = false;

void setup() {
  // put your setup code here, to run once:
  //  Serial.begin(9600);
  time = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() > (time + 300)) {
    time = millis();
    no = (++no % count);
    i = 0, j = 0;
  }
  nextLed();
}

void nextLed() {
  if (!busy) {
    busy = true;
    while (true) {
      if (letter[no][i] & (0b1 << (3 - j))) {
        mch.turnOn(i + 1, j + 1);
        increase();
        break;
      }
      increase();
    }
    busy = false;
  }
}

void increase() {
  j = ++j % 4;
  if (j == 0)
    i = (++i % 5);
}
