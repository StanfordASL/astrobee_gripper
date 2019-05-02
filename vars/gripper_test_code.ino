unsigned char LowByte(unsigned short v) {
  return ((unsigned char) (v));
}

unsigned char HighByte(unsigned short v) {
  return ((unsigned char) (((unsigned int) (v)) >> 8));
}

uint16_t ToUInt16(unsigned char* ptr) {
  uint16_t new_value = ((uint16_t)(ptr[0])<<8) | ((uint16_t)(ptr[1]));
  return new_value;
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
  received_packet[0] = 0xFF;
  received_packet[1] = 0xFF;
  received_packet[2] = 0xFD;
  received_packet[3] = 0x00;
  received_packet[4] = TARGET_GRIPPER;

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
      received_packet[ndx] = rc;

      if (ndx == 6) {
        size_t len = ((received_packet[6] << 8) | received_packet[5]);
        packet_len += len;

        if (packet_len > num_chars) {
          // Not enough space allocated in received_packet to read packet
          // Serial.print("Discarding packet as length exceeds maximum length of ");
          // Serial.println(num_chars);
          ResetState();
        }
      } else if (ndx == 7 && received_packet[7] == INSTR_WRITE) {
        send_ack_packet = true;
      } else if (ndx == packet_len - 1) {
        unsigned short packet_checksum = (((received_packet[packet_len - 1]) << 8) | ((received_packet[packet_len - 2])));        
        unsigned short crc_value = 0;
        crc_value = update_crc(crc_value, received_packet, packet_len - 2);

        if (crc_value == packet_checksum) {
          // Serial.println("Checksum matches!");
        } else {
          // Serial.println("Checksum does not match!");
          err_state = ConstructErrorByte(ERR_CRC);
          SendAckPacket();
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
        case DELAY: 
          SendGraspDelayPacket(); 
          break;
        default:
          err_state = ConstructErrorByte(ERR_INSTR_READ);
          SendAckPacket();
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
        case ADDRESS_SET_DELAY: 
          SetGraspDelay(); 
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
        default:
          err_state = ConstructErrorByte(ERR_INSTR_WRITE);
          SendAckPacket();
          break;
      }

      break;

    default:
      err_state = ConstructErrorByte(ERR_INSTR);
      SendAckPacket();
  }

  ResetState();
}

bool ReadToF() {
  // See: https://github.com/adafruit/Adafruit_VL6180X/tree/master/examples/vl6180x

  vl_range_mm = vl.readRange();
  uint8_t status = vl.readRangeStatus();
  if (status == VL6180X_ERROR_NONE) {
    return true;
  } else {
    err_state = ConstructErrorByte(ERR_TOF_READ);
    return false;
  }
}

void MeasureCurrentSensors() {
  // Take current sensor measurements
  current_L1_mA = ina219_L1.getCurrent_mA();
  current_L2_mA = ina219_L2.getCurrent_mA();
  current_R_mA = ina219_R.getCurrent_mA();
  current_W_mA = ina219_W.getCurrent_mA();
}

void setup() {
  Serial1.begin(115200);    // RS-485
  Serial.begin(115200);     // USB

  // Default gripper states
  adhesive_engage = false;
  wrist_lock = false;
  automatic_mode_enable = false;
  experiment_in_progress = false;
  gripper_open = true;
  overtemperature_flag = false;

  // Global variables
  new_data = false;
  packet_len = lead_in_len;
  ndx = 5;
  send_ack_packet = false;
  err_state = 0x00;

  // Pin to toggle RS-485 between read/write
  pinMode(UART1_DIR, OUTPUT);
  digitalWrite(UART1_DIR, LOW);

  // Set LED pins
  pinMode(LED1_R, OUTPUT);
  pinMode(LED1_G, OUTPUT);
  pinMode(LED1_B, OUTPUT);
  pinMode(LED2_R, OUTPUT);
  pinMode(LED2_G, OUTPUT);
  pinMode(LED2_B, OUTPUT);
  analogWrite(LED1_R, 0);
  analogWrite(LED1_G, LED_HIGH);
  analogWrite(LED1_B, 0);
  analogWrite(LED2_R, 0);
  analogWrite(LED2_G, 0);
  analogWrite(LED2_B, 0);
  
  // Initialize the VL6180X
  if (!vl.begin()) {
    // Set error byte
    err_state = ConstructErrorByte(ERR_TOF_INIT);
  }

  auto_grasp_write_delay_ms = 200;
  vl_range_first_trigger_set = false;
  vl_range_second_trigger_set = false;

  // Initialize the INA219.
  ina219_L1.begin();
  ina219_L2.begin();
  ina219_R.begin();
  ina219_W.begin();
  ina219_L1.setCalibration_32V_1A();
  ina219_L2.setCalibration_32V_1A();
  ina219_R.setCalibration_32V_1A();
  ina219_W.setCalibration_32V_1A();

  //initializing the PWMServoDriver
  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);
  disengage_pulse_high = false;

  //Initialize the wrist lock servo
  wrist_lock_servo.attach(20);

  file_is_open = false;
  pinMode(CS, OUTPUT);
  if (!SD.begin(CS)) {
    err_state = ConstructErrorByte(ERR_SD_INIT);
  }
}

void loop() {
  IncomingData();
  if (new_data) { 
    ProcessData();
  }

  DisengagePulseTimer();

  Automatic();
  UpdateGripper();

  IncomingData();
  if (new_data) { 
    ProcessData();
  }

  if (experiment_in_progress) {
   WriteToCard();
  }
}
