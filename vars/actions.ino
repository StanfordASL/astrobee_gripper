void OpenGripper() {
  Disengage();
  delay(50);
  Unlock();
  analogWrite(4, 0);
  analogWrite(5, LED_HIGH);
  analogWrite(6, 0);
}

void CloseGripper() {
  Engage();
  delay(50);
  Lock();
  analogWrite(4, LED_HIGH);
  analogWrite(5, 0);
  analogWrite(6, 0);
}

void ToggleAuto() {
  analogWrite(21, 0);
  analogWrite(22, 0);
  analogWrite(23, 0);

  // TODO(acauligi): do something with grippers
  if (toggle ==  1) {
    analogWrite(21, LED_HIGH);
    toggle = 2;
  } else {
    analogWrite(22, LED_HIGH);
    toggle = 1;
  }
}

void Mark() {
  return;
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

void OpenExperiment() {
  experiment_idx++;
  experiment_in_progress = true;
  return;
}

void NextRecord() {
  return;
}

void SeekRecord() {
  return;
}

void CloseExperiment() {
  experiment_in_progress = true;
  return;
}

void Automatic() {
  if (!automatic_mode_enable) {
    return;
  else if (!ReadToF()) {
    return;
  }

  if (vl_range < vl_range_trigger && !adhesive_engage && !wrist_lock) {
    OpenGripper();
  else if (vl_range > vl_range_trigger && adhesive_engage && wrist_lock) {
    CloseGripper();
  }
}
