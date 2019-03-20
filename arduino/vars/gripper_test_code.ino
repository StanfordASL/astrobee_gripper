unsigned char LowByte(unsigned short v) {
  return ((unsigned char) (v));
}

unsigned char HighByte(unsigned short v) {
  return ((unsigned char) (((unsigned int) (v)) >> 8));
}

// Send packet using UART
void SendPacket(unsigned char* packet, size_t len) {
  Serial1.write(packet, len);
  Serial.println("Something sent!");
}

unsigned char ConstructErrorPacket(char ERR_NUMBER) {
  unsigned char ERR_BYTE = (0x01 << 7) | ERR_NUMBER;
  return ERR_BYTE;
}

void SendErrorPacket(char ERR_NUMBER) {
  unsigned char ERR_BYTE = ConstructErrorPacket(ERR_NUMBER);

  size_t err_packet_len = 11;
  unsigned char err_packet[err_packet_len];
  err_packet[0] = 0xff;
  err_packet[1] = 0xff;
  err_packet[2] = 0xfd;
  err_packet[3] = 0x00;
  err_packet[4] = TARGET_GRIPPER;
  err_packet[5] = LowByte(err_packet_len - fixed_packet_len);
  err_packet[6] = HighByte(err_packet_len - fixed_packet_len);
  err_packet[7] = INSTR_STATUS;
  err_packet[8] = ERR_BYTE;

  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, err_packet, err_packet_len - 2);
  err_packet[9] = LowByte(crc_value);
  err_packet[10] = HighByte(crc_value);

  SendPacket(err_packet, err_packet_len);
}

void SendPingPacket() {
  size_t ping_packet_len = 14;
  unsigned char ping_packet[ping_packet_len];
  ping_packet[0] = 0xff;
  ping_packet[1] = 0xff;
  ping_packet[2] = 0xfd;
  ping_packet[3] = 0x00;
  ping_packet[4] = TARGET_GRIPPER;
  ping_packet[5] = LowByte(ping_packet_len - fixed_packet_len);
  ping_packet[6] = HighByte(ping_packet_len - fixed_packet_len);
  ping_packet[7] = INSTR_STATUS;
  ping_packet[8] = 0x00;

  // dummy status packet parameters
  ping_packet[9]  = 0x01;
  ping_packet[10] = 0x02;
  ping_packet[11] = 0x03;

  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, ping_packet, ping_packet_len - 2);
  ping_packet[12] = LowByte(crc_value);
  ping_packet[13] = HighByte(crc_value);

  SendPacket(ping_packet, ping_packet_len);
}

void SendAcknowledgePacket() {
  unsigned char ack_packet[packet_len];
}

// Reset state of received_packet
void ResetState() {
  new_data = false;

  memset(received_packet, 0, sizeof(received_packet));
  memset(hdr_buffer, 0, sizeof(hdr_buffer));

  packet_len = fixed_packet_len;
  ndx = 5;
  send_acknowledge_packet = false;
}

void IncomingData() {
  char rc;

  while ((Serial1.available() > 0) && (new_data == false)) {
    rc = Serial1.read();

    if ((hdr_buffer[0] != 0xff) ||
        (hdr_buffer[1] != 0xff) ||
        (hdr_buffer[2] != 0xfd) ||
        (hdr_buffer[3] != 0x00) ||
        (hdr_buffer[4] != TARGET_GRIPPER)) {
      // Overwrite existing header buffer if invalid
      for (size_t k = 0; k < hdr_size - 1; k++) {
        hdr_buffer[k] = hdr_buffer[k + 1];
      }
      hdr_buffer[hdr_size - 1] = rc;
    } else {
      for (size_t k = 0; k < hdr_size; k++) received_packet[k] = hdr_buffer[k];
      received_packet[ndx] = rc;

      if (ndx == 6) {
        size_t len = ((received_packet[6] << 8) | received_packet[5]);
        packet_len += len;

        if (packet_len > num_chars) {
          // Not enough space allocated in received_packet to read packet
          Serial.print("Discarding packet as length exceeds maximum length of ");
          Serial.println(num_chars);
          ResetState();
        }
      } else if (ndx == 7 && received_packet[7] == INSTR_WRITE) {
        send_acknowledge_packet = true;
      } else if (ndx == packet_len - 1) {
        unsigned short packet_checksum = (((received_packet[packet_len - 1]) << 8) | ((received_packet[packet_len - 2])));        
        unsigned short crc_value = 0;
        crc_value = update_crc(crc_value, received_packet, packet_len - 2);

        if (crc_value == packet_checksum) {
          Serial.println("Checksum matches!");
        } else {
          Serial.println("Checksum does not match!");
          SendErrorPacket(ERR_CRC);
          ResetState();
          break;
        }
      }
      ndx++;
      if (ndx >= packet_len) {
        new_data = true;
        break;
      }
    }
  }
}

void ProcessData() {
  if (new_data == true) {
    // for (size_t k = 0; k < packet_len; k++) Serial.print((char)(received_packet[k]));
    // Serial.println();
    // Serial.println(millis());

    if (send_acknowledge_packet) {
      SendPacket(received_packet, packet_len);
      send_acknowledge_packet = false;
    }

    char instr = received_packet[7];
    char addr = ((received_packet[9] << 8) | (received_packet[8]));
    Serial.println(instr, HEX);

    switch (instr) {
      case INSTR_PING:
        SendPingPacket();

      case INSTR_READ:
        switch (addr) {
          case STATUS:
            Serial.println("STATUS");
            break;
          case RECORD:
            Serial.println("RECORD");
            break;
          case EXPERIMENT:
            Serial.println("EXPERIMENT");
            break;
        }

        break;

      case INSTR_WRITE:
        switch (addr) {
          case ADDRESS_OPEN:
            Serial.println("ADDRESS_OPEN");
            OpenGripper();
            break;
          case ADDRESS_CLOSE:
            Serial.println("ADDRESS_CLOSE");
            CloseGripper();
            break;
          case ADDRESS_TOGGLE_AUTO:
            Serial.println("ADDRESS_TOGGLE_AUTO");
            ToggleAuto();
            break;
          case ADDRESS_MARK:
            Serial.println("ADDRESS_MARK");
            break;
          case ADDRESS_ENGAGE:
            Serial.println("ADDRESS_ENGAGE");
            break;
          case ADDRESS_DISENGAGE:
            Serial.println("ADDRESS_DISENGAGE");
            break;
          case ADDRESS_LOCK:
            Serial.println("ADDRESS_LOCK");
            break;
          case ADDRESS_UNLOCK:
            Serial.println("ADDRESS_UNLOCK");
            break;
          case ADDRESS_ENABLE_AUTO:
            Serial.println("ADDRESS_ENABLE_AUTO");
            break;
          case ADDRESS_DISABLE_AUTO:
            Serial.println("ADDRESS_DISABLE_AUTO");
            break;
          case ADDRESS_OPEN_EXPERIMENT:
            Serial.println("ADDRESS_OPEN_EXPERIMENT");
            break;
          case ADDRESS_NEXT_RECORD:
            Serial.println("ADDRESS_NEXT_RECORD");
            break;
          case ADDRESS_SEEK_RECORD:
            Serial.println("ADDRESS_SEEK_RECORD");
            break;
          case ADDRESS_CLOSE_EXPERIMENT:
            Serial.println("ADDRESS_CLOSE_EXPERIMENT");
            break;
        }

        break;

      default:
        Serial.println("ERROR PACKET");
    }

    ResetState();
  }
}

void UpdateGripperState() {
  // Read four status bytes
  adhesive_engage;
  wrist_lock;
  automatic_mode_enable;
  experiment_in_progress;
}

void setup() {
  Serial1.begin(115200);
  Serial.begin(115200);

  // Gripper states
  adhesive_engage = false;
  wrist_lock = false;
  automatic_mode_enable = false;
  experiment_in_progress = false;

  // Global variables
  new_data = false;
  packet_len = fixed_packet_len;
  ndx = 5;
  toggle = 1;
  send_acknowledge_packet = false;

  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(21, LOW);
  digitalWrite(22, LOW);
  digitalWrite(23, LOW);
}

void loop() {
  UpdateGripperState();
  IncomingData();
  ProcessData();
  // char rc = 'n';
  // if (Serial1.available() > 0) {
  // rc = Serial1.read();
  // Serial.println(rc);
  // }
}
