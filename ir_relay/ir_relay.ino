#include <IRremote.hpp>

IRrecv irrecv(4);
decode_results results;
#define reley 7
#define button1 0xFF22DD
#define button2 0xFFC23D


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  irrecv.enableIRIn();
  Serial.println("hello world");
  pinMode(reley, OUTPUT);
  digitalWrite(reley, HIGH);
  delay(2000);
  digitalWrite(reley, LOW);
  delay(2000);



}

void loop() {

  if (irrecv.decode(&results)) {
    Serial.print("CODE: ");
    Serial.println(results.value, HEX);
    
    switch(results.value){

      case button1:
          digitalWrite(reley, HIGH);
          Serial.println("button 1");
          break;

      case button2:
          digitalWrite(reley, LOW);
          Serial.println("button 2");
          break;
      }

    irrecv.resume(); 
  }

}
