unsigned char LowByte(unsigned short v) {
  return ((unsigned char) (v));
}

unsigned char HighByte(unsigned short v) {
  return ((unsigned char) (((unsigned int) (v)) >> 8));
}

uint32_t ToUInt32(char* ptr) {
  uint32_t new_value = ((uint32_t)(ptr[0])<<24) | ((uint32_t)(ptr[1])<<16) | ((uint32_t)(ptr[2])<<8) | ((uint32_t)(ptr[3])); 
  return new_value;
}

// Reset state of received_packet
void ResetState() {
  new_data = false;

  memset(received_packet, 0, sizeof(received_packet));
  memset(hdr_buffer, 0, sizeof(hdr_buffer));

  packet_len = hdr_len;
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
      for (size_t k = 0; k < hdr_const_byte_len - 1; k++) {
        hdr_buffer[k] = hdr_buffer[k + 1];
      }
      hdr_buffer[hdr_const_byte_len - 1] = rc;
    } else {
      for (size_t k = 0; k < hdr_const_byte_len; k++) received_packet[k] = hdr_buffer[k];
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
            // SendStatusPacket(); 
            break;
          case RECORD:
            Serial.println("RECORD");
            // SendRecordPacket(); 
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

bool ReadToF() {
  // See: https://github.com/adafruit/Adafruit_VL6180X/tree/master/examples/vl6180x

  vl_range = vl.readRange();
  uint8_t status = vl.readRangeStatus();
  if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
    Serial.println("System error");
  } else if (status == VL6180X_ERROR_ECEFAIL) {
    Serial.println("ECE failure");
  } else if (status == VL6180X_ERROR_NOCONVERGE) {
    Serial.println("No convergence");
  } else if (status == VL6180X_ERROR_RANGEIGNORE) {
    Serial.println("Ignoring range");
  } else if (status == VL6180X_ERROR_SNR) {
    Serial.println("Signal/Noise error");
  } else if (status == VL6180X_ERROR_RAWUFLOW) {
    Serial.println("Raw reading underflow");
  } else if (status == VL6180X_ERROR_RAWOFLOW) {
    Serial.println("Raw reading overflow");
  } else if (status == VL6180X_ERROR_RANGEUFLOW) {
    Serial.println("Range reading underflow");
  } else if (status == VL6180X_ERROR_RANGEOFLOW) {
    Serial.println("Range reading overflow");
  } else {
    return true;
  }
  return false;
}

void UpdateGripperState() {
  // Take ToF sensor measurement
  if (automatic_mode_enable) {
    if (!ReadToF()) {
      // faulty ToF sensor reading, send error byte
      // SendErrorPacket(ERR_TOF);
    }
  }

  // Take current sensor measurements
  current_mA_A = ina219_A.getCurrent_mA();
  current_mA_B = ina219_B.getCurrent_mA();
  current_mA_C = ina219_C.getCurrent_mA();
  current_mA_D = ina219_D.getCurrent_mA();

  // TODO(acauligi)
  // overtemperature_flag; 
}

void setup() {
  Serial1.begin(115200);
  Serial.begin(115200);

  // Default gripper states
  adhesive_engage = false;
  wrist_lock = false;
  automatic_mode_enable = false;
  experiment_in_progress = false;
  overtemperature_flag = false;

  // Global variables
  new_data = false;
  packet_len = hdr_len;
  ndx = 5;
  send_ack_packet = false;
  err_state = 0x00;

  // Set LED pins
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  analogWrite(4, 0);
  analogWrite(5, 0);
  analogWrite(6, LED_HIGH);
  analogWrite(21, 0);
  analogWrite(22, 0);
  analogWrite(23, LED_HIGH);
  
  // Initialize the VL6180X
  vl.begin();
  for (size_t k = 0; k < n_vel_buf; k++) {
    // fill sensor measurement buffer with null values
    vel_buf[k] = NULL;  // TODO(acauligi): or 0x00?
  }

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
  
  pinMode(chip_select, OUTPUT);
  if (!SD.begin(chip_select)) {
    Serial.println("SD card initialization failed! Trying again...");
    while (1);
  }
}

void loop() {
  UpdateGripperState();
  IncomingData();
  ProcessData();
  Automatic();

  if (experiment_in_progress) {
    WriteToCard();
  }
}
