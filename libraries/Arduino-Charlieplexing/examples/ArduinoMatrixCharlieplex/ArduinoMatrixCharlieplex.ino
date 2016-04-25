#include "MatrixCharlieplex.h"
using namespace ArduinoMatrixCharlieplex;

uint8_t Pins[] = {8, 9, 10, 11, 12};
MatrixCharlieplex mch(Pins, 5, MXCHARLIE_CA);

int i = 0, j = 0;
unsigned long time;
boolean busy = false;

void setup() {
  // put your setup code here, to run once:
//  Serial.begin(9600);
  time=millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() > (time+100)){
    time=millis();
    blink();
  }
}

void blink(){
  if(!busy){
    busy = true;
//    Serial.print("i:\t");
//    Serial.println(i);
//    Serial.print("Node:\t");
//    Serial.println(i+1);
    // for single parameter index based
    mch.turnOn(i+1);
    increase();
//    Serial.print("i-j:\t");
//    Serial.print(i);
//    Serial.print(" - ");
//    Serial.println(j);
//    Serial.print("Node:\t");
//    Serial.print(i+1);
//    Serial.print(" - ");
//    Serial.println(j+1);
    // for dual parameter matrix based
//    mch.turnOn(i+1, j+1);
//    increase2();
    busy = false;
  }
}

void increase(){
  i = (++i % 20);
}

void increase2(){
  j = ++j % 4;
  if(j == 0) i = (++i % 5);
}
