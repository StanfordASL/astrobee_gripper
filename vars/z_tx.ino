// Send packet using UART
void SendPacket(unsigned char* packet, size_t len) {
  Serial1.write(packet, len);
}

unsigned char ConstructErrorByte(char ERR_NUMBER) {
  unsigned char ERR_BYTE = (0x01 << 7) | ERR_NUMBER;
  return ERR_BYTE;
}

void SendErrorPacket(char ERR_NUMBER) {
  err_state = ConstructErrorByte(ERR_NUMBER);

  size_t err_packet_len = 11;
  unsigned char err_packet[err_packet_len];
  err_packet[0] = 0xff;
  err_packet[1] = 0xff;
  err_packet[2] = 0xfd;
  err_packet[3] = 0x00;
  err_packet[4] = TARGET_GRIPPER;
  err_packet[5] = LowByte(err_packet_len - hdr_len);
  err_packet[6] = HighByte(err_packet_len - hdr_len);
  err_packet[7] = INSTR_STATUS;
  err_packet[8] = err_state; 

  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, err_packet, err_packet_len - 2);
  err_packet[9] = LowByte(crc_value);
  err_packet[10] = HighByte(crc_value);

  SendPacket(err_packet, err_packet_len);
}

void SendPingPacket() {
  size_t ping_packet_len = 10; 
  unsigned char ping_packet[ping_packet_len];
  ping_packet[0] = 0xff;
  ping_packet[1] = 0xff;
  ping_packet[2] = 0xfd;
  ping_packet[3] = 0x00;
  ping_packet[4] = TARGET_GRIPPER;
  ping_packet[5] = LowByte(ping_packet_len - hdr_len);
  ping_packet[6] = HighByte(ping_packet_len - hdr_len);
  ping_packet[7] = INSTR_STATUS;
  ping_packet[8] = err_state;

  // dummy status packet parameters
  // ping_packet[9]  = 0x01;
  // ping_packet[10] = 0x02;
  // ping_packet[11] = 0x03;

  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, ping_packet, ping_packet_len - 2);
  ping_packet[9] = LowByte(crc_value);
  ping_packet[10] = HighByte(crc_value);

  SendPacket(ping_packet, ping_packet_len);
}

void SendAcknowledgePacket() {
  unsigned char ack_packet[packet_len];
}

void SendStatusPacket() {
  size_t status_packet_len = 13; 
  unsigned char status_packet[status_packet_len];
  status_packet[0] = 0xff;
  status_packet[1] = 0xff;
  status_packet[2] = 0xfd;
  status_packet[3] = 0x00;
  status_packet[4] = TARGET_GRIPPER;
  status_packet[5] = LowByte(status_packet_len - hdr_len);
  status_packet[6] = HighByte(status_packet_len - hdr_len);
  status_packet[7] = INSTR_STATUS;
  status_packet[8] = err_state; 

  // STATUS_H = [TEMP -   -   -   -   -   - EXP]
  unsigned char STATUS_H = (overtemperature_flag<<7) | experiment_in_progress;
  status_packet[9] = STATUS_H;

  // STATUS_L = [- - - - AUTO - WRIST ADH] 
  unsigned char STATUS_L = (automatic_mode_enable<<3) | (wrist_lock<<1) | adhesive_engage;
  status_packet[10] = STATUS_L; 

  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, status_packet, status_packet_len - 2);
  status_packet[11] = LowByte(crc_value);
  status_packet[12] = HighByte(crc_value);

  SendPacket(status_packet, status_packet_len);
}  

/*
void SendRecordPacket() {
  size_t record_packet_len = 46; 
  unsigned char record_packet[record_packet_len];
  record_packet[0] = 0xff;
  record_packet[1] = 0xff;
  record_packet[2] = 0xfd;
  record_packet[3] = 0x00;
  record_packet[4] = TARGET_GRIPPER;
  record_packet[5] = LowByte(record_packet_len - hdr_len);
  record_packet[6] = HighByte(record_packet_len - hdr_len);
  record_packet[7] = INSTR_STATUS;
  record_packet[8] = err_state; 
  size_t txIdx = 9;



  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, record_packet, record_packet_len - 2);
  record_packet[43] = LowByte(crc_value);
  record_packet[44] = HighByte(crc_value);

  SendPacket(record_packet, record_packet_len);
}
*/

void SendExperimentPacket() {
  size_t experiment_packet_len = 9 + exp_record_len + 2;  
  unsigned char experiment_packet[experiment_packet_len];
  experiment_packet[0] = 0xff;
  experiment_packet[1] = 0xff;
  experiment_packet[2] = 0xfd;
  experiment_packet[3] = 0x00;
  experiment_packet[4] = TARGET_GRIPPER;
  experiment_packet[5] = LowByte(experiment_packet_len - hdr_len);
  experiment_packet[6] = HighByte(experiment_packet_len - hdr_len);
  experiment_packet[7] = INSTR_STATUS;
  experiment_packet[8] = err_state; 
 
  for (size_t exp_idx = 0; exp_idx < exp_record_len; exp_idx++) {
    experiment_packet[9+exp_idx] = exp_line[exp_idx];
  }

  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, experiment_packet, experiment_packet_len - 2);
  experiment_packet[9+exp_record_len] = LowByte(crc_value);
  experiment_packet[10+exp_record_len] = HighByte(crc_value);

  SendPacket(experiment_packet, experiment_packet_len);
}
