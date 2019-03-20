void setup() {
  Serial1.begin(115200);
  Serial.begin(115200);

  // Global variables 
  new_data = false;
  packet_len = fixed_packet_len;
  ndx = 5;
}

void loop() {
  ResetState();
  // SendPingPacket();
  // SendReadPacket();
  // SendWritePacket();
  SendChecksumPacket();
  Serial.println("The packet has been sent!");
  IncomingData();
  if (new_data) {
    bool packet_valid = VerifyChecksumRxPacket();
    if (packet_valid) {
      Serial.println("Test passed!");
    } else {
      Serial.println("Test failed!");
    }
  }
  delay(3000);
}
