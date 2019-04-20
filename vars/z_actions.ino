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
  UpdateGripperState();
  automatic_mode_enable = true;
  return;
}

void DisableAuto() {
  automatic_mode_enable = false;
  return;
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

  experiment_idx = ToUInt32(received_packet+lead_in_len+1);

  String fn = String(experiment_idx + ".txt");
  for (int i = 0; i < file_open_attempts; i++) {
    my_file = SD.open(fn, FILE_WRITE);
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

  experiment_idx = ToUInt32(received_packet+lead_in_len+1);
  
  String fn = String(experiment_idx + ".txt");
  for (int i = 0; i < file_open_attempts; i++) {
    my_file = SD.open(fn, FILE_READ);
    if (my_file) {
      file_is_open = true;
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

  uint8_t skip = (uint8_t)(received_packet[8]); 

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
    record_num = ((uint32_t)(received_packet[8])<<24) | ((uint32_t)(received_packet[9])<<16) | ((uint32_t)(received_packet[10])<<8) | ((uint32_t)(received_packet[11])); 
  } else {
    // TODO(acauligi): Send error byte?
  }
  return;
}

void CloseExperiment() {
  if(!file_is_open || !experiment_in_progress) {
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

  // TODO(acauligi): change buffer into circular buffer
  if (vl_range_mm < vl_range_max_mm && vl_range_mm > vl_range_min_mm) {
    if(vel_buf_idx==0) {
      last_vl_range_mm = vl_range_mm;
      last_vl_range_time_ms = millis(); 
    } else {
      if (vel_buf_idx==n_vel_buf) {
        // Buffer is full
        for (size_t k = 0; k < n_vel_buf-1; k++) {
          vel_buf[k] = vel_buf[k+1];
        }
        vel_buf_idx = n_vel_buf-1;
      }

      cur_time_ms = millis();
      vel_buf[vel_buf_idx] = (vl_range_mm-last_vl_range_mm) / (cur_time_ms - last_vl_range_time_ms);
      last_vl_range_mm = vl_range_mm;
      last_vl_range_time_ms = cur_time_ms;
    }
    vel_buf_idx++;
  } else if (vl_range_mm < vl_range_min_mm) {
    // calculate average speed from values in buffer
    float mean_vel_mps = 0;
    if (!vel_buf[0]) {
      return;
    } else {
      // TODO(acauligi): double check!
      mean_vel_mps = vel_buf[0];
      size_t k = 1;
      while (k<n_vel_buf && !vel_buf[k]) {
        mean_vel_mps = (k*mean_vel_mps + vel_buf[k]) / (k+1);
        k++;
      }
    }

    float dh = vl_range_mm / mean_vel_mps; 
    delay(dh);
    CloseGripper();
  }
}
