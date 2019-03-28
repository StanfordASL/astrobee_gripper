void OpenGripper() {
  digitalWrite(4, HIGH);
  digitalWrite(5, LOW);
}

void CloseGripper() {
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
}

void ToggleAuto() {
  if (toggle ==  1) {
    digitalWrite(21, HIGH);
    toggle = 2;
  } else {
    digitalWrite(22, HIGH);
    toggle = 1;
  }
}

void Mark() {
  return;
}

void Engage() {
  // engage the pull tendons
  pwm.setPWM(5,0,370);
  pwm.setPWM(6,0,268);

  Serial.println("Engage");
  //update the status register
  adhesive_engage = true;
  return;
}

void Disengage() {
  //disengage the pull tendons
  pwm.setPWM(5,0,300);
  pwm.setPWM(6,0,310);
  
  //pulse the realese tendons
  pwm.setPWM(7,0,210);
  delay(50);
  pwm.setPWM(7,0,300);

  Serial.println("Disengage");
  //update the status register
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
  return;
}

void DisableAuto() {
  return;
}

void OpenExperiment() {
  return;
}

void NextRecord() {
  return;
}

void SeekRecord() {
  return;
}

void CloseExperiment() {
  return;
}
