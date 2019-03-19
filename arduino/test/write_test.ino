size_t write_tx_packet_len = 10;
unsigned char write_tx_packet[write_tx_packet_len];

size_t write_rx_packet_len = 11;
unsigned char write_rx_packet[write_rx_packet_len];

void ConstructRxPacket() {
  write_rx_packet[0] = 0xff;
  write_rx_packet[1] = 0xff;
  write_rx_packet[2] = 0xfd;
  write_rx_packet[3] = 0x00;
  write_rx_packet[4] = TARGET_GRIPPER; 
  write_rx_packet[5] = LowByte(write_rx_packet_len - 7);
  write_rx_packet[6] = HighByte(write_rx_packet_len - 7);
  write_rx_packet[7] = INSTR_STATUS;
  write_rx_packet[8] = 0x00;
        
  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, write_rx_packet, write_rx_packet_len - 2);
  write_rx_packet[9] = LowByte(crc_value); 
  write_rx_packet[10] = HighByte(crc_value); 
}

void SendWritePacket() {
  write_tx_packet[0] = 0xff;
  write_tx_packet[1] = 0xff;
  write_tx_packet[2] = 0xfd;
  write_tx_packet[3] = 0x00;
  write_tx_packet[4] = TARGET_GRIPPER; 
  write_tx_packet[5] = LowByte(write_tx_packet_len - 7);
  write_tx_packet[6] = HighByte(write_tx_packet_len - 7);
  write_tx_packet[7] = INSTR_WRITE;
  write_tx_packet[8] = 0x00;
        
  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, write_tx_packet, write_tx_packet_len - 2);
  write_tx_packet[9] = LowByte(crc_value); 
  write_tx_packet[10] = HighByte(crc_value); 

  SendPacket(write_tx_packet, write_tx_packet_len);
}

bool VerifyRxPacket(unsigned char received_packet) {
  bool passed = true;
  if (sizeof(received_packet) != write_rx_packet_len) {
    passed = false;
  } else {
    for (size_t k = 0; k < write_rx_packet_len; k++) {
      if (received_packet[k] != write_rx_packet[k]) {
        passed = false;
      }
    }
  }
  return passed;
}

void setup() {
  Serial1.begin(115200);
  Serial.begin(115200);

  // Global variables 
  new_data = false;
  packet_len = fixed_packet_len;
  ndx = 5;
}

void loop() {
  SendWritePacket();
  IncomingData();
  if (new_data) {
    bool packet_valid = VerifyRxPacket();
    if (packet_valid) {
      Serial.println("Write test passed!");
    } else {
      Serial.println("Write test failed!");
    }
  }
}
