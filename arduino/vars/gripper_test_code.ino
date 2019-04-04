unsigned char LowByte(unsigned short v) {
  return ((unsigned char) (v));
}

unsigned char HighByte(unsigned short v) {
  return ((unsigned char) (((unsigned int) (v)) >> 8));
}

// Send packet using UART
void SendPacket(unsigned char* packet, size_t len) {
  Serial1.write(packet, len);
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
  ping_packet[8] = 0x00;    // no error byte

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

void SendStatusPacket() {
  size_t status_packet_len = 12;
  unsigned char status_packet[status_packet_len];
  status_packet[0] = 0xff;
  status_packet[1] = 0xff;
  status_packet[2] = 0xfd;
  status_packet[3] = 0x00;
  status_packet[4] = TARGET_GRIPPER;
  status_packet[5] = LowByte(status_packet_len - fixed_packet_len);
  status_packet[6] = HighByte(status_packet_len - fixed_packet_len);
  status_packet[7] = INSTR_STATUS;

  // STATUS_H = [- - - - AUTO - WRIST ADH] 
  unsigned char STATUS_H = (automatic_mode_enable<<3) | (wrist_lock<<1) | (adhesive_engage);
  status_packet[8] = STATUS_H;

  // STATUS_L = [TEMP -   -   -   -   -   - EXP]
  unsigned char STATUS_L = (overtemperature_flag<<7) | experiment_in_progress;
  status_packet[9] = STATUS_L; 

  unsigned short crc_value = 0;
  crc_value = update_crc(crc_value, status_packet, status_packet_len - 2);
  status_packet[10] = LowByte(crc_value);
  status_packet[11] = HighByte(crc_value);

  SendPacket(status_packet, status_packet_len);
}  

// void SendRecordPacket() {
//   size_t record_packet_len = 45; 
//   unsigned char record_packet[record_packet_len];
//   record_packet[0] = 0xff;
//   record_packet[1] = 0xff;
//   record_packet[2] = 0xfd;
//   record_packet[3] = 0x00;
//   record_packet[4] = TARGET_GRIPPER;
//   record_packet[5] = LowByte(record_packet_len - fixed_packet_len);
//   record_packet[6] = HighByte(record_packet_len - fixed_packet_len);
//   record_packet[7] = INSTR_STATUS;
//   size_t txIdx = 8;
// 
//   // TIME: running 16-bit unsigned counter since Teensy power-on
//   record_packet[txIdx+0] =; 
//   record_packet[txIdx+1] =;
//   record_packet[txIdx+2] =;
//   record_packet[txIdx+3] =;
//   record_packet[txIdx+4] =;
// 
//   record_packet[txIdx+5] = ',';
// 
//   if (adhesive_engage) {
//     record_packet[txIdx+6] ='E';
//   } else {
//     record_packet[txIdx+6] ='D';
//   }
// 
//   if (wrist_lock) { 
//     record_packet[txIdx+7] = 'L';
//   } else {
//     record_packet[txIdx+7] = 'U';
//   }
// 
//   if (automatic_mode_enable) { 
//     record_packet[txIdx+8] = 'A';
//   } else {
//     record_packet[txIdx+8] = '*';
//   }
// 
//   record_packet[txIdx+9] = ',';
// 
//   // SRV_L1_CURR
//   record_packet[txIdx+10] =
//   record_packet[txIdx+11] =
//   record_packet[txIdx+12] =
//   record_packet[txIdx+13] =
// 
//   record_packet[txIdx+14] = ',';
// 
//   // SRV_L2_CURR
//   record_packet[txIdx+15] = ;
//   record_packet[txIdx+16] = ;
//   record_packet[txIdx+17] = ;
//   record_packet[txIdx+18] = ;
// 
//   record_packet[txIdx+19] = ',';
// 
//   // SRV_R_CURR 
//   record_packet[txIdx+20] = ;
//   record_packet[txIdx+21] = ;
//   record_packet[txIdx+22] = ;
//   record_packet[txIdx+23] = ;
// 
//   record_packet[txIdx+24] = ',';
// 
//   // SRV_W_CURR 
//   record_packet[txIdx+25] = ;
//   record_packet[txIdx+26] = ;
//   record_packet[txIdx+27] = ;
//   record_packet[txIdx+28] = ;
// 
//   record_packet[txIdx+29] = ',';
// 
//   // TOF
//   record_packet[txIdx+30] = ;
//   record_packet[txIdx+31] = ;
//   record_packet[txIdx+32] = ;
//   
//   record_packet[txIdx+33] = ',';
//   
//   
//   if (overtemperature_flag) {
//     record_packet[txIdx+34] = '*';
//   } else {
//     record_packet[txIdx+34] = '-';
//   }
// 
//   unsigned short crc_value = 0;
//   crc_value = update_crc(crc_value, record_packet, record_packet_len - 2);
//   record_packet[43] = LowByte(crc_value);
//   record_packet[44] = HighByte(crc_value);
// 
//   SendPacket(record_packet, record_packet_len);
// }

// Reset state of received_packet
void ResetState() {
  new_data = false;

  memset(received_packet, 0, sizeof(received_packet));
  memset(hdr_buffer, 0, sizeof(hdr_buffer));

  packet_len = fixed_packet_len;
  ndx = 5;
  send_ack_packet = false;
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
        send_ack_packet = true;
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

    if (send_ack_packet) {
      // SendPacket(received_packet, packet_len);
      // TODO(acauligi): send back ack packet with data in write command
      SendPingPacket();
      send_ack_packet = false;
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
  // adhesive_engage;
  // wrist_lock;
  // automatic_mode_enable;
  // experiment_in_progress;
  // overtemperature_flag; 

  current_mA_A = ina219_A.getCurrent_mA();
  current_mA_B = ina219_B.getCurrent_mA();
  current_mA_C = ina219_C.getCurrent_mA();
  current_mA_D = ina219_D.getCurrent_mA();
}

void WriteToCard() {
//  my_file = SD.open("test_write.txt", FILE_WRITE);
//  if (my_file) {
//    my_file.println(current_mA_A);
//    my_file.println(current_mA_B);
//    my_file.println(current_mA_C);
//    my_file.println(current_mA_D);
//    my_file.close();
//    // Serial.println("Writing to SD card");
//  } else {
//    // Serial.println("Not writing to SD card");
//  }
//    while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  my_file = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (my_file) {
    Serial.print("Writing to test.txt...");
     my_file.println("testing 1, 2, 3.");
    my_file.println(current_mA_A);
    my_file.println(current_mA_B);
    my_file.println(current_mA_C);
    my_file.println(current_mA_D);
    // close the file:
    my_file.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // re-open the file for reading:
  my_file = SD.open("test.txt");
  if (my_file) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (my_file.available()) {
      Serial.write(my_file.read());
    }
    // close the file:
    my_file.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void setup() {
  Serial1.begin(115200);
  Serial.begin(115200);

  // Gripper states
  adhesive_engage = false;
  wrist_lock = false;
  automatic_mode_enable = false;
  experiment_in_progress = false;
  overtemperature_flag = false;

  // Global variables
  new_data = false;
  packet_len = fixed_packet_len;
  ndx = 5;
  toggle = 1;
  send_ack_packet = false;

  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
//  digitalWrite(4, LOW);
//  digitalWrite(5, LOW);
//  digitalWrite(6, HIGH);
//  digitalWrite(21, LOW);
//  digitalWrite(22, LOW);
//  digitalWrite(23, HIGH);
  analogWrite(4, 0);
  analogWrite(5, 0);
  analogWrite(6, LED_HIGH);
  analogWrite(21, 0);
  analogWrite(22, 0);
  analogWrite(23, LED_HIGH);
  

  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  ina219_A.begin();
  ina219_B.begin();
  ina219_C.begin();
  ina219_D.begin();
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  ina219_A.setCalibration_32V_1A();
  ina219_B.setCalibration_32V_1A();
  ina219_C.setCalibration_32V_1A();
  ina219_D.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

  //initializing the PWMServoDriver
  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);

  //Initialize the wrist lock servo
  wrist_lock_servo.attach(20);

}

void loop() {
  UpdateGripperState();
  IncomingData();
  ProcessData();
  WriteToCard();
}
