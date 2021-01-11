#ifndef _MEAW_H
#define _MEAW_H

//#define DEBUG 
#ifdef DEBUG
 #define debug(x,y) if (y) Serial.println (x); else Serial.print(x)
#else
 #define debug(x,y)
#endif

// Bluetooth 
#define BLUETOOTHSPEED 115200//460800 //9600//115200

// General
#define NUMOPS 4
enum operations {ERR, MOTOR, ACK, STOP};
  uint8_t tam_msg[NUMOPS] = { 3, 6, 4, 4 }; // ERR, MOTOR, ACK, STOP

//Buffer
#define BUFF_SIZE 256

typedef struct {
  uint8_t buf[BUFF_SIZE];
  uint8_t bbeg;
  uint8_t bend;
  int8_t bcnt;
} sbuffer;

#define AUDIO_SIZE 4
typedef struct {
  uint8_t abeg;
  uint8_t aend;
} audio_cell;

typedef struct {
  audio_cell aud[AUDIO_SIZE];
  uint8_t acurr;
  uint8_t acnt;
} abuffer; 


//TODO
// Define valores para velocidades horário e antihorario
#define VEL_MIN_HORA 0
#define VEL_MAX_HORA 127
#define VEL_MIN_ANTI 129
#define VEL_MAX_ANTI 255
#define MAP_MIN 0
#define MAP_MAX 255


#endif
