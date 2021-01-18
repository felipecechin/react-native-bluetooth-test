
sbuffer b;
uint8_t crc;

void setup_scomm() {
  Serial.begin(BLUETOOTHSPEED); 
  while(!Serial);

  b.buf[0] = '\0';
  b.bbeg = 0;
  b.bend = 0;
  b.bcnt = 0;
}

void show_buffer(){
  debug("\nBuffer bs=",0);
  debug(b.bcnt,0);
  debug(" | beg=",0);
  debug(b.bbeg,0);
  debug(" | bend=",0);
  debug(b.bend,0);
  debug(" | buf=",0);
  uint8_t i=b.bbeg;
  for (uint8_t j=0; j<b.bcnt; j++){
    debug(b.buf[i],0);
    debug(" ",0);
    i=i+1>=BUFF_SIZE?0:i+1;
  }
  debug("",1);
}

void loop_scomm() {
  if (Serial.available() > 0) {

    b.buf[b.bend] = Serial.read();
    debug("\nReceived: ",0);
    debug(b.buf[b.bend],1);
    b.bend=b.bend+1>=BUFF_SIZE?0:b.bend+1;
    b.bcnt++;
    show_buffer();
    
  }
  analyses_buffer();
}

void send_ack(uint8_t ack_number){
  uint8_t msg_ack[2]={ACK, ack_number};
  for (uint8_t i=0; i<2; i++)
    Serial.print(msg_ack[i]);
  Serial.print("\r\n");
/*TODO: CRC  uint8_t msg_ack[6]={ACK, ack_number, 0, 0,'\r','\n'};
  msg_ack[2]=crc8_buf(msg_ack, 2);
  msg_ack[3]=~msg_ack[2];
  for (uint8_t i=0; i<6; i++)
    Serial.print(msg_ack[i]);
*/
}

uint8_t buff_get_byte(uint8_t pos){
  uint8_t p = b.bbeg+pos>=BUFF_SIZE?(b.bbeg+pos)%BUFF_SIZE:b.bbeg+pos;
  return b.buf[p];
}

static void parse_msg(uint8_t cod_msg) {
  // ERR
  // | 8 bits | 8 bits | 8 bits |  -> 3 bytes
  // | 0 (cod)|  crc   |  ~crc  |  //cod_err?
  if (cod_msg == ERR) {
    debug("Got an error!",1);
    return;
  }
  // ML
  // | 8 bits | 8 bits | 8 bits | 8 bits | 8 bits | 8 bits |  -> 6 bytes
  // | 1 (cod)| speed h| speed v| acknum |  crc   |  ~crc  |
  else if (cod_msg == MOTOR) { // motors
    uint8_t h = buff_get_byte(+1);
    uint8_t v = buff_get_byte(+2);
    uint8_t ack = buff_get_byte(+3);
    run_motors(h, v);
    debug("RunMotors",0);
    debug(h,0);
    debug(",",0);
    debug(v,1);
    send_ack(ack);
  }
  // STOP
  // | 8 bits | 8 bits | 8 bits | 8 bits |  -> 4 bytes
  // | 0 (cod)| ackNum |  crc   |  ~crc  |  
  else if (cod_msg == STOP) { // stop motors
    uint8_t ack = buff_get_byte(+1);
    stop_motors();
    debug("StopMotors",1);
    send_ack(ack);
  }
}


bool decode_1() {
  for (;;) {
    if (b.bcnt < tam_msg[0]) // Verifies if buffer has at least the minimum message size (error msg)
      return false;
    
    uint8_t cod_msg = b.buf[b.bbeg];
    
    if (cod_msg != ERR && cod_msg != MOTOR && cod_msg != STOP) { // Not a good message code
      // Go to the next byte in array 
      b.bbeg=b.bbeg+1>=BUFF_SIZE?0:b.bbeg+1; 
      b.bcnt=b.bcnt-1<0?0:b.bcnt-1;
      continue;
    }

    if (b.bcnt < tam_msg[cod_msg]) // Verifies if the buffer lenght has at least the message size
      return false;
      
    if ((buff_get_byte(tam_msg[cod_msg] - 2) ^ buff_get_byte(tam_msg[cod_msg] - 1)) != 0xff) { 
      // Ignores message if CRC is not good
      debug("CRC Failed!",1);
      b.bbeg=b.bbeg+tam_msg[cod_msg]>=BUFF_SIZE?(b.bbeg+tam_msg[cod_msg])%BUFF_SIZE:b.bbeg+tam_msg[cod_msg];
      b.bcnt=b.bcnt-tam_msg[cod_msg]<0?0:b.bcnt-tam_msg[cod_msg];
      return false;
    } 
    
    // Looks like we have a good message
    debug("A good one!",1);
    show_buffer();
    parse_msg(cod_msg);
    // next circular buffer begining position
    b.bbeg=b.bbeg+tam_msg[cod_msg]>=BUFF_SIZE?(b.bbeg+tam_msg[cod_msg])%BUFF_SIZE:b.bbeg+tam_msg[cod_msg];
    b.bcnt=b.bcnt-tam_msg[cod_msg]<0?0:b.bcnt-tam_msg[cod_msg];
    break;
  }
  return true;
}

void analyses_buffer(){
  // smaller package has 3 bytes
    uint8_t cod_msg = b.buf[b.bbeg];
    
    if (cod_msg != ERR && cod_msg != MOTOR && cod_msg != STOP) { // Not a good message code
      b.bbeg=b.bbeg+1>=BUFF_SIZE?0:b.bbeg+1; 
      b.bcnt=b.bcnt-1<0?0:b.bcnt-1;
      return;
    }

  if (b.bcnt < tam_msg[0]) return;

  while (decode_1());
  
  if (b.bcnt == 0) {
    // buffer is empty
  } else if ((b.bcnt+BUFF_SIZE*.1) > BUFF_SIZE) {
    // buffer almost full
    debug("Buffer almost full!",1);
  }
}
