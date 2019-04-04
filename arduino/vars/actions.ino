void OpenGripper() {
  Disengage();
  delay(50);
  Unlock();
//  digitalWrite(4, LOW);
//  digitalWrite(5, HIGH);
//  digitalWrite(6, LOW);
  analogWrite(4, 0);
  analogWrite(5, LED_HIGH);
  analogWrite(6, 0);
}

void CloseGripper() {
  Engage();
  delay(50);
  Lock();
//  digitalWrite(4, HIGH);
//  digitalWrite(5, LOW);
//  digitalWrite(6, LOW);
  analogWrite(4, LED_HIGH);
  analogWrite(5, 0);
  analogWrite(6, 0);
}

void ToggleAuto() {
//  digitalWrite(21, LOW);
//  digitalWrite(22, LOW);
//  digitalWrite(23, LOW);
  analogWrite(21, 0);
  analogWrite(22, 0);
  analogWrite(23, 0);

  if (toggle ==  1) {
//    digitalWrite(21, HIGH);
    analogWrite(21, LED_HIGH);
    toggle = 2;
  } else {
//    digitalWrite(22, HIGH);
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
  //update the status register
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
