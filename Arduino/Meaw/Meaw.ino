#include "Meaw.h"

void setup(){
  setup_scomm();
  setup_motor();
  debug("Hello from meaw!",1);
}
  
void loop(){
  /*
  for (int i=0;i<=10;i++){
    left_motor(FORWARD, 64+i*19);
    right_motor(FORWARD, 64+i*19);
    delay(1500);
  }
  stop_right();
  stop_left();
  delay(2500);
  */
  //loop_bluetooth();
  loop_scomm();
  //Serial.println("oi");
  //delay(1000);
/*  for(int i=0; i<256; i+=10){
    //run_motors(i,i);
    left_motor(i);
    right_motor(i);
    Serial.println(i);
    delay(1000);
  }*/
}
