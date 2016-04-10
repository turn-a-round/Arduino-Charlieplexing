#include "MatrixCharlieplex.h"
using namespace ArduinoMatrixCharlieplex;

uint8_t Pins[] = {8, 9, 10, 11, 12};
MatrixCharlieplex mch(Pins, 5, MXCHARLIE_CA);

DiodeNode* node;
//int c=0;
int i = 0, j = 0;//, count = 0;
unsigned long time;
int letter[] = {
  0b1110,
  0b1001,
  0b1110,
  0b1001,
  0b1110
  };
boolean busy = false;

void setup() {
  // put your setup code here, to run once:
  //mch = new MatrixCharlieplex(Pins);
  //Serial.begin(9600);
  //mch = MatrixCharlieplex(Pins, 5, MXCHARLIE_CA);
//  pinMode(8, OUTPUT);
//  pinMode(9, OUTPUT);
//  Pins2 = mch.GetPins();
//  for(int x=0;x<5;x++){
//    Serial.print(*(Pins2+x));
//    Serial.print("-");
//  }
//  Serial.println("--");
// i = 0;
// j = 0;
  time=millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() > (time+100)){
    time=millis();
    blink();
//    count++;
//    if(count > 1000)
//      mch.Reset();
  }
//  Serial.print(i);
//  Serial.print("-");
//  Serial.println(j);
//  for(int i=1; i<=5; i++){
//    for(int j=1; j<5; j++){
//      mch.TurnOn(i, j);
////      node=mch.GetActiveNode();
////      Serial.println(node->vcc);
////      Serial.println(node->gnd);
////      if(c%5==0){
////        Pins2 = mch.GetPins();
////        for(int x=0;x<5;x++){
////          Serial.print(*(Pins2+x));
////          Serial.print("-");
////        }
////        Serial.println("--");
////      }
////      c++;
//      //delay(1000);
//    }
//  }

//  digitalWrite(8, HIGH);   // turn the LED on (HIGH is the voltage level)
//  digitalWrite(9, LOW); 
//  delay(1000);              // wait for a second
//  digitalWrite(9, HIGH);   // turn the LED on (HIGH is the voltage level)
//  digitalWrite(8, LOW); 
//  delay(1000);              // wait for a second
  //delay(1000);
}

void blink(){
  if(!busy){
    busy = true;
//  //if((j == 2 || j == 3) && (i == 2 || i == 3))
//  mch.TurnOn(i+1, j+1);
//  //else
//  //mch.Clear();
//  j = ++j % 4;
//  if(j == 0) i = (++i % 5);
//    while(true){
//      if(letter[i] & (0b1<<(3-j))){
//        if(count == 0)
//          mch.Clear();
//        count = ++count % 20;
//      mch.TurnOn(i+1, j+1);
//      increase();
//      break;
//      }
//      increase();
//  //    else
//  //    mch.Clear();
//    }
    //mch.TurnOn(i+1, j+1);
//    Serial.print("i:\t");
//    Serial.println(i);
//    Serial.print("Node:\t");
//    Serial.println(i+1);
    mch.TurnOn(i+1);
    //increase();
    increase2();
    busy = false;
  }
}

void increase(){
  j = ++j % 4;
  if(j == 0) i = (++i % 5);
}

void increase2(){
  i = (++i % 4);
}

