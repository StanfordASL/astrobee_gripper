// size_t checksum_tx_packet_len = 10;
// unsigned char checksum_tx_packet[checksum_tx_packet_len];
// 
// size_t checksum_rx_packet_len = 11;
// unsigned char checksum_rx_packet[checksum_rx_packet_len];
// 
// void ConstructRxPacket() {
//   checksum_rx_packet[0] = 0xff;
//   checksum_rx_packet[1] = 0xff;
//   checksum_rx_packet[2] = 0xfd;
//   checksum_rx_packet[3] = 0x00;
//   checksum_rx_packet[4] = TARGET_GRIPPER; 
//   checksum_rx_packet[5] = LowByte(checksum_rx_packet_len - 7);
//   checksum_rx_packet[6] = HighByte(checksum_rx_packet_len - 7);
//   checksum_rx_packet[7] = INSTR_STATUS;
// 
//   unsigned char ERROR_BYTE = 0x01;
//   ERROR_BYTE = ERROR_BYTE << 7;
//   ERROR_BYTE = ERROR_BYTE | 0x03;
// 
//   checksum_rx_packet[8] = ERROR_BYTE; 
//         
//   unsigned short crc_value = 0;
//   crc_value = update_crc(crc_value, checksum_rx_packet, checksum_rx_packet_len - 2);
//   checksum_rx_packet[9] = LowByte(crc_value); 
//   checksum_rx_packet[10] = HighByte(crc_value); 
// }
// 
// void SendChecksumPacket() {
//   checksum_tx_packet[0] = 0xff;
//   checksum_tx_packet[1] = 0xff;
//   checksum_tx_packet[2] = 0xfd;
//   checksum_tx_packet[3] = 0x00;
//   checksum_tx_packet[4] = TARGET_GRIPPER; 
//   checksum_tx_packet[5] = LowByte(checksum_tx_packet_len - 7);
//   checksum_tx_packet[6] = HighByte(checksum_tx_packet_len - 7);
//   checksum_tx_packet[7] = INSTR_READ;
//   checksum_tx_packet[8] = 0x00;
//         
//   unsigned short crc_value = 0;
//   crc_value = update_crc(crc_value, checksum_tx_packet, checksum_tx_packet_len - 2);
//   checksum_tx_packet[9] = (LowByte(crc_value) >> 2);
//   checksum_tx_packet[10] = (HighByte(crc_value) >> 2); 
// 
//   SendPacket(checksum_tx_packet, checksum_tx_packet_len);
// }
// 
// bool VerifyRxPacket(unsigned char received_packet) {
//   bool passed = true;
//   if (sizeof(received_packet) != checksum_rx_packet_len = 11) {
//     passed = false;
//   } else {
//     for (size_t k = 0; k < checksum_rx_packet_len; k++) {
//       if (received_packet[k] != checksum_rx_packet[k]) {
//         passed = false;
//       }
//     }
//   }
//   return passed;
// }
// 
// void setup() {
//   Serial1.begin(115200);
//   Serial.begin(115200);
// 
//   // Global variables 
//   new_data = false;
//   packet_len = fixed_packet_len;
//   ndx = 5;
// }
// 
// void loop() {
//   SendChecksumPacket();
//   IncomingData();
//   if (new_data) {
//     bool packet_valid = VerifyRxPacket();
//     if (packet_valid) {
//       Serial.println("Checksum test passed!");
//     } else {
//       Serial.println("Checksum test failed!");
//     }
//   }
// }
