const size_t read_tx_packet_len = 11;
const size_t read_rx_packet_len = 11;

void ConstructReadRxPacket(unsigned char* read_rx_packet) {
  read_rx_packet[0] = 0xff;
  read_rx_packet[1] = 0xff;
  read_rx_packet[2] = 0xfd;
  read_rx_packet[3] = 0x00;
  read_rx_packet[4] = TARGET_GRIPPER; 
  read_rx_packet[5] = LowByte(read_rx_packet_len - fixed_packet_len);
  read_rx_packet[6] = HighByte(read_rx_packet_len - fixed_packet_len);
  read_rx_packet[7] = INSTR_STATUS;
  read_rx_packet[8] = 0x00;
        
  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, read_rx_packet, read_rx_packet_len - 2);
  read_rx_packet[9] = LowByte(crc_value); 
  read_rx_packet[10] = HighByte(crc_value); 
}

void SendReadPacket() {
  unsigned char read_tx_packet[read_tx_packet_len];
  read_tx_packet[0] = 0xff;
  read_tx_packet[1] = 0xff;
  read_tx_packet[2] = 0xfd;
  read_tx_packet[3] = 0x00;
  read_tx_packet[4] = TARGET_GRIPPER; 
  read_tx_packet[5] = LowByte(read_tx_packet_len - fixed_packet_len);
  read_tx_packet[6] = HighByte(read_tx_packet_len - fixed_packet_len);
  read_tx_packet[7] = INSTR_READ;
  read_tx_packet[8] = 0x00;
        
  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, read_tx_packet, read_tx_packet_len - 2);
  read_tx_packet[9] = LowByte(crc_value); 
  read_tx_packet[10] = HighByte(crc_value); 

  SendPacket(read_tx_packet, read_tx_packet_len);
}

bool VerifyReadRxPacket() {
  bool passed = true;
  unsigned char read_rx_packet[read_rx_packet_len];
  ConstructReadRxPacket(read_rx_packet);
  
  if (packet_len != read_rx_packet_len) {
    passed = false;
  } else {
    for (size_t k = 0; k < read_rx_packet_len; k++) {
      if (received_packet[k] != read_rx_packet[k]) {
        passed = false;
      }
    }
  }
  return passed;
}