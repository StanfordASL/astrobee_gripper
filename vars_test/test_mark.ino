const size_t mark_experiment_tx_packet_len = 16;
const size_t open_experiment_tx_packet_len = 16;
const size_t seek_record_tx_packet_len = 13;
const size_t next_record_tx_packet_len = 13;

void SendMarkExperimentPacket() {
  unsigned char mark_experiment_tx_packet[mark_experiment_tx_packet_len];
  mark_experiment_tx_packet[0] = 0xff;
  mark_experiment_tx_packet[1] = 0xff;
  mark_experiment_tx_packet[2] = 0xfd;
  mark_experiment_tx_packet[3] = 0x00;
  mark_experiment_tx_packet[4] = TARGET_GRIPPER; 
  mark_experiment_tx_packet[5] = LowByte(mark_experiment_tx_packet_len - fixed_packet_len);
  mark_experiment_tx_packet[6] = HighByte(mark_experiment_tx_packet_len - fixed_packet_len);
  mark_experiment_tx_packet[7] = INSTR_WRITE;
  mark_experiment_tx_packet[8] = LowByte(ADDRESS_MARK);
  mark_experiment_tx_packet[9] = HighByte(ADDRESS_MARK);

  // uint32_t IDX;
  mark_experiment_tx_packet[10] = 0x00; // ((unsigned char) ( ( ((unsigned long) IDX) && 0xFF000000UL) >> 24));
  mark_experiment_tx_packet[11] = 0x00; // ((unsigned char) ( ( ((unsigned long) IDX) && 0x00FF0000UL) >> 16));
  mark_experiment_tx_packet[12] = 0x23; // ((unsigned char) ( ( ((unsigned long) IDX) && 0x0000FF00UL) >> 8));
  mark_experiment_tx_packet[13] = 0x28; // ((unsigned char) ( ( ((unsigned long) IDX) && 0x00000000UL)));
        
  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, mark_experiment_tx_packet, mark_experiment_tx_packet_len - 2);
  mark_experiment_tx_packet[14] = LowByte(crc_value); 
  mark_experiment_tx_packet[15] = HighByte(crc_value); 

  SendPacket(mark_experiment_tx_packet, mark_experiment_tx_packet_len);
}

void SendOpenExperimentPacket() {
  unsigned char send_open_experiment_tx_packet[open_experiment_tx_packet_len];
  send_open_experiment_tx_packet[0] = 0xff;
  send_open_experiment_tx_packet[1] = 0xff;
  send_open_experiment_tx_packet[2] = 0xfd;
  send_open_experiment_tx_packet[3] = 0x00;
  send_open_experiment_tx_packet[4] = TARGET_GRIPPER; 
  send_open_experiment_tx_packet[5] = LowByte(mark_experiment_tx_packet_len - fixed_packet_len);
  send_open_experiment_tx_packet[6] = HighByte(mark_experiment_tx_packet_len - fixed_packet_len);
  send_open_experiment_tx_packet[7] = INSTR_WRITE;
  send_open_experiment_tx_packet[8] = LowByte(ADDRESS_OPEN_EXPERIMENT);
  send_open_experiment_tx_packet[9] = HighByte(ADDRESS_OPEN_EXPERIMENT);

  // uint32_t IDX;
  send_open_experiment_tx_packet[10] = 0x00; // ((unsigned char) ( ( ((unsigned long) IDX) && 0xFF000000UL) >> 24));
  send_open_experiment_tx_packet[11] = 0x00; // ((unsigned char) ( ( ((unsigned long) IDX) && 0x00FF0000UL) >> 16));
  send_open_experiment_tx_packet[12] = 0x00; // ((unsigned char) ( ( ((unsigned long) IDX) && 0x0000FF00UL) >> 8));
  send_open_experiment_tx_packet[13] = 0x09; // ((unsigned char) ( ( ((unsigned long) IDX) && 0x00000000UL)));
        
  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, send_open_experiment_tx_packet, open_experiment_tx_packet_len - 2);
  send_open_experiment_tx_packet[14] = LowByte(crc_value); 
  send_open_experiment_tx_packet[15] = HighByte(crc_value); 

  SendPacket(send_open_experiment_tx_packet, open_experiment_tx_packet_len);
}

void SendSeekRecordPacket() {
  unsigned char send_seek_record_tx_packet[seek_record_tx_packet_len];
  send_seek_record_tx_packet[0] = 0xff;
  send_seek_record_tx_packet[1] = 0xff;
  send_seek_record_tx_packet[2] = 0xfd;
  send_seek_record_tx_packet[3] = 0x00;
  send_seek_record_tx_packet[4] = TARGET_GRIPPER; 
  send_seek_record_tx_packet[5] = LowByte(mark_experiment_tx_packet_len - fixed_packet_len);
  send_seek_record_tx_packet[6] = HighByte(mark_experiment_tx_packet_len - fixed_packet_len);
  send_seek_record_tx_packet[7] = INSTR_WRITE;
  send_seek_record_tx_packet[8] = LowByte(ADDRESS_SEEK_RECORD);
  send_seek_record_tx_packet[9] = HighByte(ADDRESS_SEEK_RECORD);

  // uint32_t RN;
  send_seek_record_tx_packet[10] = 0x00; // ((unsigned char) ( ( ((unsigned long) IDX) && 0xFF000000UL) >> 24));
  send_seek_record_tx_packet[11] = 0x00; // ((unsigned char) ( ( ((unsigned long) IDX) && 0x00FF0000UL) >> 16));
  send_seek_record_tx_packet[12] = 0x00; // ((unsigned char) ( ( ((unsigned long) IDX) && 0x0000FF00UL) >> 8));
  send_seek_record_tx_packet[13] = 0x09; // ((unsigned char) ( ( ((unsigned long) IDX) && 0x00000000UL)));
        
  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, send_seek_record_tx_packet, seek_record_tx_packet_len - 2);
  send_seek_record_tx_packet[14] = LowByte(crc_value); 
  send_seek_record_tx_packet[15] = HighByte(crc_value); 

  SendPacket(send_seek_record_tx_packet, seek_record_tx_packet_len);
}

void SendNextRecordPacket() {
  unsigned char send_next_record_tx_packet[next_record_tx_packet_len];
  send_next_record_tx_packet[0] = 0xff;
  send_next_record_tx_packet[1] = 0xff;
  send_next_record_tx_packet[2] = 0xfd;
  send_next_record_tx_packet[3] = 0x00;
  send_next_record_tx_packet[4] = TARGET_GRIPPER; 
  send_next_record_tx_packet[5] = LowByte(mark_experiment_tx_packet_len - fixed_packet_len);
  send_next_record_tx_packet[6] = HighByte(mark_experiment_tx_packet_len - fixed_packet_len);
  send_next_record_tx_packet[7] = INSTR_WRITE;
  send_next_record_tx_packet[8] = LowByte(ADDRESS_NEXT_RECORD);
  send_next_record_tx_packet[9] = HighByte(ADDRESS_NEXT_RECORD);

  send_next_record_tx_packet[10] = 0x00; // ((unsigned char) ( ( ((unsigned long) IDX) && 0xFF000000UL) >> 24));
        
  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, send_next_record_tx_packet, next_record_tx_packet_len - 2);
  send_next_record_tx_packet[11] = LowByte(crc_value); 
  send_next_record_tx_packet[12] = HighByte(crc_value); 

  SendPacket(send_next_record_tx_packet, next_record_tx_packet_len);
}
