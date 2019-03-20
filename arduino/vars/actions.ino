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
  return;
}

void Disengage() {
  return;
}

void Lock() {
  return;
}

void Unlock() {
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
