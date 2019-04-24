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

  switch (Serial.read()) {
    case 109:  // 'm'
      SendMarkExperimentPacket();
      Serial.println("Mark has been sent");
      break;

    case 99: // 'c'
      SendCloseExperimentPacket();
      Serial.println("Close experiment sent");
      break;

    case 114: // 'r'
      SendReadPacket();
      break;

    case 111: // 'o'
      SendOpenExperimentPacket();
      Serial.println("Open experiment sent");
      break;

    case 79: // 'O'
      SendOpenPacket();
      Serial.println("Open command sent");
      break;

    case 67: // 'C'
      SendClosePacket();
      Serial.println("Close command sent");
      break;

    case 101: // 'e'
      SendAutomaticEnablePacket();
      Serial.println("Automatic Enable sent");
      break;
  }

  //  SendPingPacket();
  //   SendReadPacket();
  //   SendWritePacket();
  //   SendChecksumPacket();
  //  SendOpenPacket();
  //  delay(1500);
  //  SendClosePacket();
  //  Serial.println("The packet has been sent!");

  IncomingData();
  if (new_data) {
    Serial.print("packet_len: ");
    Serial.println(packet_len);
    for (size_t k = 0; k < packet_len; k++) {
      Serial.print(char(received_packet[k]));
    }
    Serial.println();
    //    bool packet_valid = VerifyChecksumRxPacket();
    //    if (packet_valid) {
    //      Serial.println("Test passed!");
    //    } else {
    //      Serial.println("Test failed!");
    //    }
  }
  //  delay(1500);
}
