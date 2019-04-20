unsigned char LowByte(unsigned short v) {
  return ((unsigned char) (v));
}

unsigned char HighByte(unsigned short v) {
  return ((unsigned char) (((unsigned int) (v)) >> 8));
}

uint32_t ToUInt32(unsigned char* ptr) {
  uint32_t new_value = ((uint32_t)(ptr[0])<<24) | ((uint32_t)(ptr[1])<<16) | ((uint32_t)(ptr[2])<<8) | ((uint32_t)(ptr[3])); 
  return new_value;
}

// Reset state of received_packet
void ResetState() {
  // TODO(acauligi): update with all new variables that have been created
  new_data = false;

  memset(received_packet, 0, sizeof(received_packet));
  memset(const_byte_buffer, 0, sizeof(const_byte_buffer));

  packet_len = lead_in_len;
  ndx = 5;
  send_ack_packet = false;
}

void IncomingData() {
  char rc;

  while ((Serial1.available() > 0) && (new_data == false)) {
    rc = Serial1.read();

    if ((const_byte_buffer[0] != 0xFF) ||
        (const_byte_buffer[1] != 0xFF) ||
        (const_byte_buffer[2] != 0xFD) ||
        (const_byte_buffer[3] != 0x00) ||
        (const_byte_buffer[4] != TARGET_GRIPPER)) {
      // Overwrite existing header buffer if invalid
      for (size_t k = 0; k < packet_const_byte_len - 1; k++) {
        const_byte_buffer[k] = const_byte_buffer[k + 1];
      }
      const_byte_buffer[packet_const_byte_len - 1] = rc;
    } else {
      for (size_t k = 0; k < packet_const_byte_len; k++) received_packet[k] = const_byte_buffer[k];
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
  char instr = received_packet[7];
  char addr = ((received_packet[9] << 8) | (received_packet[8]));

  if (send_ack_packet) {
    SendAckPacket();
    send_ack_packet = false;
  }

  switch (instr) {
    case INSTR_PING:
      SendAckPacket();
      break;

    case INSTR_READ:
      switch (addr) {
        case STATUS:
          SendStatusPacket(); 
          break;
        case RECORD:
          SendRecordPacket(); 
          break;
        case EXPERIMENT:
          SendExperimentPacket();
          break;
      }

      break;

    case INSTR_WRITE:
      switch (addr) {
        case ADDRESS_OPEN:
          OpenGripper();
          break;
        case ADDRESS_CLOSE:
          CloseGripper();
          break;
        case ADDRESS_TOGGLE_AUTO:
          ToggleAuto();
          break;
        case ADDRESS_MARK:
          Mark();
          break;
        case ADDRESS_ENGAGE:
          Engage();
          break;
        case ADDRESS_DISENGAGE:
          Disengage();
          break;
        case ADDRESS_LOCK:
          Lock();
          break;
        case ADDRESS_UNLOCK:
          Unlock();
          break;
        case ADDRESS_ENABLE_AUTO:
          EnableAuto();
          break;
        case ADDRESS_DISABLE_AUTO:
          DisableAuto();
          break;
        case ADDRESS_OPEN_EXPERIMENT:
          OpenExperiment();
          break;
        case ADDRESS_NEXT_RECORD:
          NextRecord();
          break;
        case ADDRESS_SEEK_RECORD:
          SeekRecord();
          break;
        case ADDRESS_CLOSE_EXPERIMENT:
          CloseExperiment();
          break;
      }

      break;

    default:
      Serial.println("ERROR PACKET");
  }

  ResetState();
}

bool ReadToF() {
  // See: https://github.com/adafruit/Adafruit_VL6180X/tree/master/examples/vl6180x

  vl_range_mm = vl.readRange();
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
  packet_len = lead_in_len;
  ndx = 5;
  send_ack_packet = false;
  err_state = 0x00;

  // Pin to toggle RS-485 between read/write
  pinMode(UART1_DIR, OUTPUT);
  digitalWrite(2, LOW);

  // Set LED pins
  pinMode(LED1_R, OUTPUT);
  pinMode(LED1_G, OUTPUT);
  pinMode(LED1_B, OUTPUT);
  pinMode(LED2_R, OUTPUT);
  pinMode(LED2_G, OUTPUT);
  pinMode(LED2_B, OUTPUT);
  analogWrite(LED1_R, LED_HIGH);
  analogWrite(LED1_G, LED_HIGH);
  analogWrite(LED1_B, LED_HIGH);
  analogWrite(LED2_R, LED_HIGH);
  analogWrite(LED2_G, LED_HIGH);
  analogWrite(LED2_B, LED_HIGH);
  
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
  
  pinMode(CS, OUTPUT);
  if (!SD.begin(CS)) {
    Serial.println("SD card initialization failed! Trying again...");
    while (1);
  }
}

void loop() {
  // TODO(acauligi): update loop() to run faster to not drop packets
  UpdateGripperState();
  IncomingData();
  if (new_data) { 
    ProcessData();
  }

 Automatic();

 if (experiment_in_progress) {
   WriteToCard();
 }
}
