void OpenGripper() {
  Disengage();
  delay(50);
  Unlock();
  analogWrite(LED2_R, 0);
  analogWrite(LED2_G, LED_HIGH);
  analogWrite(LED2_B, 0);
}

void CloseGripper() {
  Engage();
  delay(50);
  Lock();
  analogWrite(LED2_R, LED_HIGH);
  analogWrite(LED2_G, 0);
  analogWrite(LED2_B, 0);
}

void Engage() {
  // engage the pull tendons
  pwm.setPWM(5,0,335);
  pwm.setPWM(6,0,195);

  Serial.println("Engage");
  adhesive_engage = true;
  return;
}

void Disengage() {
  //disengage the pull tendons
  pwm.setPWM(5,0,200);
  pwm.setPWM(6,0,320);
  
  //pulse the realese tendons
  pwm.setPWM(7,0,210);
  delay(50);
  pwm.setPWM(7,0,300);

  Serial.println("Disengage");
  adhesive_engage = false;
  return;
}

void Lock() {
  wrist_lock_servo.write(35);
  wrist_lock = true; 
  return;
}

void Unlock() {
  wrist_lock_servo.write(70);  
  wrist_lock = false; 
  return;
}

void EnableAuto() {
  OpenGripper();
  automatic_mode_enable = true;
  return;
}

void DisableAuto() {
  automatic_mode_enable = false;
  return;
}

void EnableAstronautDelay() {
  add_astronaut_delay = true; 
}

void DisableAstronautDelay() {
  add_astronaut_delay = false; 
}

void ToggleAuto() {
  if (automatic_mode_enable) {
    automatic_mode_enable = false;
  // TODO(acauligi): if (experiment_in_progress || file_is_open), should something be done?
  } else {
    automatic_mode_enable = true;
  }
  return;
}

void Mark() {
  if (experiment_in_progress) {
    Serial.println("Experiment already in session! Cannot MARK new one.");
    // TODO(acauligi): Send error byte?
    return;
  }

  // TODO(acauligi): better way to check for this?
  if (packet_len < min_rx_len+experiment_packet_data_len) {
    return;
  }

  experiment_idx = ToUInt16(received_packet+lead_in_len+3);

  String fn = String(String(experiment_idx) + ".txt");
  char file_name[9];      // ____.txt
  fn.toCharArray(file_name,9);

  for (int i = 0; i < file_open_attempts; i++) {
    my_file = SD.open(file_name, FILE_WRITE);
    if (my_file) {
      file_is_open = true;
      experiment_in_progress = true;
      return;
    }
  }
  return;
}

void OpenExperiment() {
  if (experiment_in_progress) {
    Serial.println("Experiment already in session! Cannot Open one.");
    // TODO(acauligi): Send error byte?
    return;
  } else if (file_is_open) {
    Serial.println("File already open! Cannot Open one.");
    // TODO(acauligi): Send error byte?
    return;
  }

  // TODO(acauligi): better way to check for this?
  if (packet_len < min_rx_len+experiment_packet_data_len) {
    return;
  }

  experiment_idx = ToUInt16(received_packet+lead_in_len+1);
 
  String fn = String(String(experiment_idx) + ".txt");
  char file_name[9];      // ____.txt
  fn.toCharArray(file_name,9);
  
  for (int i = 0; i < file_open_attempts; i++) {
    my_file = SD.open(file_name, FILE_READ);
    if (my_file) {
      file_is_open = true;
      record_num = 0;
      return;
    }
  }
  return;
}

void NextRecord() {
  // TODO(acauligi): better way to check for this?
  if (packet_len < min_tx_len+1) {
    return;
  }

  uint8_t skip = (uint8_t)(received_packet[10]); 

  if (file_is_open && !experiment_in_progress) {
    record_num += skip;
    // ReadRecordFromCard();
    // SendExperimentPacket();
  } else {
    // TODO(acauligi): Send error byte?
  }
  return;
}

void SeekRecord() {
  if (packet_len < min_rx_len+experiment_packet_data_len) {
    return;
  }
  
  if (file_is_open && !experiment_in_progress) { 
    record_num = ToUInt16(received_packet+lead_in_len+3);
  } else {
    // TODO(acauligi): Send error byte?
  }
  return;
}

void CloseExperiment() {
  if(!file_is_open) { 
    // TODO(acauligi): Send error byte?
    return;
  }

  // TODO(acauligi): assign experiment_idx=0?

  my_file.close();
  experiment_in_progress = false;
  file_is_open = false;
  return;
}

void Automatic() {
  if (!automatic_mode_enable) {
    return;
  } else if (!ReadToF()) {
    return;
  }

  if (vl_range_mm < vl_range_max_mm && vl_range_mm > vl_range_min_mm && !vl_range_first_set) {
    vl_range_first_mm = vl_range_mm;
    vl_range_first_time_ms = millis();
    vl_range_first_set = true;
  } else if (vl_range_mm < vl_range_min_mm) {
    
    float v_mps = ((float)vl_range_first_mm - (float)vl_range_mm) / (millis() - vl_range_first_time_ms) ;
    float dh_ms = auto_tof_sensor_offset_mm/v_mps;
    if (add_astronaut_delay) {
      dh_ms += astronaut_delay_ms; 
    }
    
    delay(dh_ms+auto_grasp_offset_ms);
    CloseGripper();
    DisableAuto();
    vl_range_first_set = false;
  }
}
