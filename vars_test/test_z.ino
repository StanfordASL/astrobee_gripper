void setup() {
  Serial1.begin(115200);
  Serial.begin(115200);

  pinMode(51, OUTPUT);
  digitalWrite(51, HIGH);

  // Global variables
  new_data = false;
  packet_len = fixed_packet_len;
  ndx = 5;
}

void loop() {
  ResetState();
  //  SendPingPacket();
  //   SendReadPacket();
  //   SendWritePacket();
  //   SendChecksumPacket();
  SendOpenPacket();
  delay(1500);
  SendClosePacket();
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
  delay(1500);
}
