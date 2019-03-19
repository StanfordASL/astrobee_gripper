size_t ping_tx_packet_len = 11;
unsigned char ping_tx_packet[ping_tx_packet_len];

size_t ping_rx_packet_len = 14;
unsigned char ping_rx_packet[ping_rx_packet_len];

void ConstructRxPacket() {
  ping_rx_packet[0] = 0xff;
  ping_rx_packet[1] = 0xff;
  ping_rx_packet[2] = 0xfd;
  ping_rx_packet[3] = 0x00;
  ping_rx_packet[4] = TARGET_GRIPPER; 
  ping_rx_packet[5] = LowByte(ping_rx_packet_len - 7);
  ping_rx_packet[6] = HighByte(ping_rx_packet_len - 7);
  ping_rx_packet[7] = INSTR_STATUS;
  ping_rx_packet[8] = 0x00;

  // dummy status packet parameters
  ping_rx_packet[9]  = 0x01;
  ping_rx_packet[10] = 0x02;
  ping_rx_packet[11] = 0x03;
        
  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, ping_rx_packet, ping_rx_packet_len - 2);
  ping_rx_packet[12] = LowByte(crc_value); 
  ping_rx_packet[13] = HighByte(crc_value); 
}

void SendPingPacket() {
  ping_tx_packet[0] = 0xff;
  ping_tx_packet[1] = 0xff;
  ping_tx_packet[2] = 0xfd;
  ping_tx_packet[3] = 0x00;
  ping_tx_packet[4] = TARGET_GRIPPER; 
  ping_tx_packet[5] = LowByte(ping_tx_packet_len - 7);
  ping_tx_packet[6] = HighByte(ping_tx_packet_len - 7);
  ping_tx_packet[7] = INSTR_PING;
  ping_tx_packet[8] = 0x01;
        
  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, ping_tx_packet, ping_tx_packet_len - 2);
  ping_tx_packet[9] = LowByte(crc_value); 
  ping_tx_packet[10] = HighByte(crc_value); 

  SendPacket(ping_tx_packet, ping_tx_packet_len);
}

bool VerifyRxPacket() {
  bool passed = true;
  if (sizeof(received_packet) != ping_rx_packet_len) {
    passed = false;
  } else {
    for (size_t k = 0; k < ping_rx_packet_len; k++) {
      if (received_packet[k] != ping_rx_packet[k]) {
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
  SendPingPacket();
  IncomingData();
  if (new_data) {
    bool packet_valid = VerifyRxPacket();
    if (packet_valid) {
      Serial.println("Ping test passed!");
    } else {
      Serial.println("Ping test failed!");
    }
  }
}
