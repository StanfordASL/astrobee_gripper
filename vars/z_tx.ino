// Send packet using UART
void SendPacket(unsigned char* packet, size_t len) {
  // Set RS485 direction to OUTPUT
  digitalWrite(UART1_DIR, HIGH);
  delayMicroseconds(5);

  // Serial1.write(packet, len);
  for (size_t k = 0; k < len; k++) {
    Serial1.write(packet[k]);
    delayMicroseconds(2);
  }
  Serial1.flush();

  // delayMicroseconds(1);
  delay(1);

  // Set RS485 direction to INPUT
  digitalWrite(UART1_DIR, LOW);
  delayMicroseconds(5);
}

unsigned char ConstructErrorByte(char ERR_NUMBER) {
  unsigned char ERR_BYTE = (0x01 << 7) | ERR_NUMBER;
  return ERR_BYTE;
}

void ConstructExperimentRecordLine() {
  // TIME: running 16-bit unsigned counter since Teensy power-on
  cur_time_ms = millis();
  String TIME_s = String(int(cur_time_ms/1000));
  TIME_s.toCharArray((char*)record_line,5);

  record_line[5] = ',';

  record_line[6] = adhesive_engage ? 'E' : 'D';

  record_line[7] = wrist_lock ? 'L' : 'U';

  record_line[8] = automatic_mode_enable ? 'A' : '*';

  record_line[9] = ',';

  // SRV_L1_CURR
  String SRV_L1_CURR_mA = String(int(current_mA_A));
  SRV_L1_CURR_mA.toCharArray((char*)record_line+10,4);

  record_line[14] = ',';

  // SRV_L2_CURR
  String SRV_L2_CURR_mA = String(int(current_mA_B));
  SRV_L2_CURR_mA.toCharArray((char*)record_line+15,4);

  record_line[19] = ',';

  // SRV_R_CURR
  String SRV_R_CURR_mA = String(int(current_mA_C));
  SRV_R_CURR_mA.toCharArray((char*)record_line+20,4);

  record_line[24] = ',';

  // SRV_W_CURR
  String SRV_W_CURR_mA = String(int(current_mA_D));
  SRV_W_CURR_mA.toCharArray((char*)record_line+25,4);

  record_line[29] = ',';

  // TOF
  String TOF_mm = String(int(vl_range_mm));
  TOF_mm.toCharArray((char*)record_line+30,3);

  record_line[33] = ',';
  
  record_line[34] = overtemperature_flag ? '*' : '-';
}

void SendErrorPacket(unsigned char ERR_NUMBER) {
  err_state = ConstructErrorByte(ERR_NUMBER);

  size_t err_packet_len = min_tx_len;
  unsigned char err_packet[err_packet_len];
  err_packet[0] = 0xFF;
  err_packet[1] = 0xFF;
  err_packet[2] = 0xFD;
  err_packet[3] = 0x00;
  err_packet[4] = TARGET_GRIPPER;
  err_packet[5] = LowByte(err_packet_len - lead_in_len);
  err_packet[6] = HighByte(err_packet_len - lead_in_len);
  err_packet[7] = INSTR_STATUS;
  err_packet[8] = err_state; 

  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, err_packet, err_packet_len - 2);
  err_packet[err_packet_len-2] = LowByte(crc_value);
  err_packet[err_packet_len-1] = HighByte(crc_value);

  SendPacket(err_packet, err_packet_len);
}

void SendAckPacket() {
  size_t ping_packet_len = min_tx_len; 
  unsigned char ping_packet[ping_packet_len];
  ping_packet[0] = 0xFF;
  ping_packet[1] = 0xFF;
  ping_packet[2] = 0xFD;
  ping_packet[3] = 0x00;
  ping_packet[4] = TARGET_GRIPPER;
  ping_packet[5] = LowByte(ping_packet_len - lead_in_len);
  ping_packet[6] = HighByte(ping_packet_len - lead_in_len);
  ping_packet[7] = INSTR_STATUS;
  ping_packet[8] = err_state;

  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, ping_packet, ping_packet_len - 2);
  ping_packet[ping_packet_len-2] = LowByte(crc_value);
  ping_packet[ping_packet_len-1] = HighByte(crc_value);

  SendPacket(ping_packet, ping_packet_len);
}

void SendStatusPacket() {
  size_t status_packet_len = min_tx_len + status_packet_data_len; 
  unsigned char status_packet[status_packet_len];
  status_packet[0] = 0xFF;
  status_packet[1] = 0xFF;
  status_packet[2] = 0xFD;
  status_packet[3] = 0x00;
  status_packet[4] = TARGET_GRIPPER;
  status_packet[5] = LowByte(status_packet_len - lead_in_len);
  status_packet[6] = HighByte(status_packet_len - lead_in_len);
  status_packet[7] = INSTR_STATUS;
  status_packet[8] = err_state;

  // STATUS_H = [TEMP -   -   -   -   -   - EXP]
  unsigned char STATUS_H = ((char)overtemperature_flag<<7) | (char)experiment_in_progress;
  status_packet[9] = STATUS_H;
  
  // STATUS_L = [- - - - AUTO - WRIST ADH]
  unsigned char STATUS_L = (automatic_mode_enable<<3) | (wrist_lock<<1) | adhesive_engage;
  status_packet[10] = STATUS_L;

  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, status_packet, status_packet_len - 2);
  status_packet[status_packet_len-2] = LowByte(crc_value);
  status_packet[status_packet_len-1] = HighByte(crc_value);

  SendPacket(status_packet, status_packet_len);
}  

void SendRecordPacket() {
  size_t record_packet_len = min_tx_len + record_packet_data_len; 
  unsigned char record_packet[record_packet_len];
  record_packet[0] = 0xFF;
  record_packet[1] = 0xFF;
  record_packet[2] = 0xFD;
  record_packet[3] = 0x00;
  record_packet[4] = TARGET_GRIPPER;
  record_packet[5] = LowByte(record_packet_len - lead_in_len);
  record_packet[6] = HighByte(record_packet_len - lead_in_len);
  record_packet[7] = INSTR_STATUS;
  record_packet[8] = err_state; 

  // ReadRecordFromCard();
  ConstructExperimentRecordLine();
  for (size_t record_idx = 0; record_idx < record_packet_data_len; record_idx++) {
    record_packet[9+record_idx] = record_line[record_idx];
  }

  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, record_packet, record_packet_len - 2);
  record_packet[record_packet_len-2] = LowByte(crc_value);
  record_packet[record_packet_len-1] = HighByte(crc_value);

  SendPacket(record_packet, record_packet_len);
}

void SendExperimentPacket() {
  size_t experiment_packet_len = min_tx_len + experiment_packet_data_len; 
  unsigned char experiment_packet[experiment_packet_len];
  experiment_packet[0] = 0xFF;
  experiment_packet[1] = 0xFF;
  experiment_packet[2] = 0xFD;
  experiment_packet[3] = 0x00;
  experiment_packet[4] = TARGET_GRIPPER;
  experiment_packet[5] = LowByte(experiment_packet_len - lead_in_len);
  experiment_packet[6] = HighByte(experiment_packet_len - lead_in_len);
  experiment_packet[7] = INSTR_STATUS;
  experiment_packet[8] = err_state; 

  if (experiment_in_progress) {
    experiment_packet[9]  = (char)(((unsigned long)experiment_idx & 0xFF000000UL) >> 24);
    experiment_packet[10] = (char)(((unsigned long)experiment_idx & 0x00FF0000UL) >> 16);
    experiment_packet[11] = (char)(((unsigned long)experiment_idx & 0x0000FF00UL) >> 8); 
    experiment_packet[12] = (char)(((unsigned long)experiment_idx & 0x000000FFUL)     );
  } else {
    experiment_packet[9]  = 0x00; 
    experiment_packet[10] = 0x00;
    experiment_packet[11] = 0x00;
    experiment_packet[12] = 0x00;
  }

  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, experiment_packet, experiment_packet_len - 2);
  experiment_packet[experiment_packet_len-2]   = LowByte(crc_value);
  experiment_packet[experiment_packet_len-1] = HighByte(crc_value);

  SendPacket(experiment_packet, experiment_packet_len);
}
