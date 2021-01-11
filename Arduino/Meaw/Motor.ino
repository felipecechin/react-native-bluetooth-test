#include "Meaw.h"

//Left Motor
uint8_t MOT_IN1 = 7;
uint8_t MOT_IN2 = 6;
uint8_t MOT_PWMA = 9;

//Right Motor
uint8_t MOT_IN3 = 4;
uint8_t MOT_IN4 = 2;
uint8_t MOT_PWMB = 3;

void setup_motor(){
  //Left Motor
  pinMode(MOT_IN1, OUTPUT);
  pinMode(MOT_IN2, OUTPUT);
  pinMode(MOT_PWMA, OUTPUT);

  //Right Motor
  pinMode(MOT_IN3, OUTPUT);
  pinMode(MOT_IN4, OUTPUT);
  pinMode(MOT_PWMB, OUTPUT);  
}

int convert_speed(int spd){
  //return (((spd>0?spd:-spd)*19) + 64);
  return spd>0?spd:-spd;
}

void run_motors(uint8_t val1, uint8_t val2){
  uint8_t dir = val2&128;
  int spd = val2&127;
  float ang = val1 * 0.003921569; // same as val1/255
  if (dir == 0){ // go forward
    spd-=128;
  }
  int spdl = -(spd+(-255*(ang-0.5)));
  int spdr = -(spd+(-255*(0.5-ang)));
  if (dir == 0){ // go forward
    left_motor(spdl);
    right_motor(spdr);
  }
  else {
    left_motor(spdr);
    right_motor(spdl);
  }
  //Serial.print(".............................speed=");
  //Serial.print(spd);
  //Serial.print(", ang=");
  //Serial.println(ang);
}

void left_motor(int speed){
  //Serial.print("L:");
  //Serial.print(speed);
  if (speed)
    run_left_motor(speed);
  else
    stop_left_motor();  
}

void right_motor(int speed){
  //Serial.print(" R:");
  //Serial.println(speed);
  if (speed)
    run_right_motor(speed);
  else
    stop_right_motor();  
}

void run_left_motor(int speed){
  if (speed > 0){
    digitalWrite(MOT_IN1, HIGH);
    digitalWrite(MOT_IN2, LOW);
  }
  else{
    digitalWrite(MOT_IN1, LOW);
    digitalWrite(MOT_IN2, HIGH);    
  }
  analogWrite(MOT_PWMA, convert_speed(speed));
}


void run_right_motor(int speed){
  if (speed > 0){
    digitalWrite(MOT_IN4, HIGH);
    digitalWrite(MOT_IN3, LOW);
  }
  else{
    digitalWrite(MOT_IN4, LOW);
    digitalWrite(MOT_IN3, HIGH);    
  }
  analogWrite(MOT_PWMB, convert_speed(speed));
}

void stop_motors(){
  stop_left_motor();
  stop_right_motor();
}

void stop_left_motor(){
  digitalWrite(MOT_IN1, HIGH);
  digitalWrite(MOT_IN2, HIGH);
}

void stop_right_motor(){
  digitalWrite(MOT_IN3, HIGH);
  digitalWrite(MOT_IN4, HIGH);
}
