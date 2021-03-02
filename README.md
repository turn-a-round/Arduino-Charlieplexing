# Arduino-Charlieplexing
A library for Arduino to control Charlieplexed LED matrices

Drive (N * (N-1)) number of "Charlieplxed" diode matrices
(e.g. LED Matrix) with N number of outputs capable of tri-state.
The LED Matrix design is inspired from http://wealoneonearth.blogspot.com/2013/03/design-note-charlieplexing-led-matrices.html

           C A T H O D E
           1 2 3 4 5 . N
      A  1 X 0 0 0 0
      N  2 0 X 0 0 0
      O  3 0 0 X 0 0
      D  4 0 0 0 X 0
      E  5 0 0 0 0 X
         .           X
         N             X

 There will be N number of rows and (N-1) number of columns. 
The same line cannot be connected to both Anode & Cathode of the same node. 
Hence, the "X" is marked, where they cannot be both at same time.

It's best practice to connect the Anodes in same row & then connect the Cathodes.
e.g. Cathode Connection for a 5 pin setup

         (-)C A T H O D E
    R  1 -> 2 3 4 5
    O  2 -> 1 3 4 5
    W  3 -> 1 2 4 5
    S  4 -> 1 2 3 5
       5 -> 1 2 3 4

Copy the Arduino-Charlieplexing folder inside libraries to your own arduino-ide --> libraries folder to use this library.

This library is helpful if the LED array(or array of array) is visualized as a matrix (x,y) formation. (Note: here all indexing starts from 0. Hence, the first LED pointer will be (0,0) [in matrix notation] or 0 [in array notation])
e.g 
           (A 4x4 matrix|position notation)

            *n|(x,y) [Array Index|Matrix Index]*
            0|(0,0)  1|(0,1)  2|(0,2)  3|(0,3)
            4|(1,0)  5|(1,1)  6|(1,2)  7|(1,3)
            8|(2,0)  9|(2,1) 10|(2,2) 11|(2,3)
           12|(3,0) 13|(3,1) 14|(3,2) 15|(3,3)

There are two main functions: 
1. turnOn: To turn a LED on for a given position. Can be supplied with either the matrix-position (x,y) or n'th position. e.g. testMatrix.turnOn(row, column) or  testMatrix.turnOn(position)
2. turnOff: To turn a LED off for a given position.same rule as above

### Example

```cpp
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

int i = 0, j = 0;
unsigned long time;
boolean busy = false;

void setup() {
  // put your setup code here, to run once:
  time = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() > (time + 100)) {
    time = millis();
    nextLed();
  }
}

// turn on 1 LED at a time at a regular interval
void nextLed() {
  if (!busy) {
    busy = true;

    /// for single parameter index based
    mch.turnOn(i + 1);
    increase();

    /// for dual parameter matrix based
    //    mch.turnOn(i+1, j+1);
    //    increase2();
    busy = false;
  }
}

void increase() {
  i = (++i % 20);
}

void increase2() {
  j = ++j % 4;
  if (j == 0)
    i = (++i % 5);
}
```
