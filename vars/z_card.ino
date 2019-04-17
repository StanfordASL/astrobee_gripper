void WriteToCard() {
  UpdateGripperState(); 

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  String fn = String(experiment_idx + ".txt");
  my_file = SD.open(fn, FILE_WRITE);

  // if the file opened okay, write to it:
  if (my_file) {
    size_t record_packet_len = 35; 
    unsigned char record_packet[record_packet_len];
    // TIME: running 16-bit unsigned counter since Teensy power-on
    time = millis();
    record_packet[0] = 0x00; 
    record_packet[1] = (char)(((unsigned long)time & 0xff000000UL) >> 24);
    record_packet[2] = (char)(((unsigned long)time & 0x00ff0000UL) >> 16);
    record_packet[3] = (char)(((unsigned long)time & 0x0000ff00UL) >> 8); 
    record_packet[4] = (char)(((unsigned long)time & 0x000000ffUL)     ); 

    record_packet[5] = ',';

    record_packet[6] = adhesive_engage ? 'E' : 'D';

    record_packet[7] = wrist_lock ? 'L' : 'U';

    record_packet[8] = automatic_mode_enable ? 'A' : '*';

    record_packet[9] = ',';

    // SRV_L1_CURR
    record_packet[10] = (char)(((unsigned long)current_mA_A & 0xff000000UL) >> 24);
    record_packet[11] = (char)(((unsigned long)current_mA_A & 0x00ff0000UL) >> 16);
    record_packet[12] = (char)(((unsigned long)current_mA_A & 0x0000ff00UL) >> 8); 
    record_packet[13] = (char)(((unsigned long)current_mA_A & 0x000000ffUL)     );

    record_packet[14] = ',';

    // SRV_L2_CURR
    record_packet[15] = (char)(((unsigned long)current_mA_B & 0xff000000UL) >> 24);
    record_packet[16] = (char)(((unsigned long)current_mA_B & 0x00ff0000UL) >> 16);
    record_packet[17] = (char)(((unsigned long)current_mA_B & 0x0000ff00UL) >> 8); 
    record_packet[18] = (char)(((unsigned long)current_mA_B & 0x000000ffUL)     ); 

    record_packet[19] = ',';

    // SRV_R_CURR 
    record_packet[20] = (char)(((unsigned long)current_mA_C & 0xff000000UL) >> 24);
    record_packet[21] = (char)(((unsigned long)current_mA_C & 0x00ff0000UL) >> 16);
    record_packet[22] = (char)(((unsigned long)current_mA_C & 0x0000ff00UL) >> 8); 
    record_packet[23] = (char)(((unsigned long)current_mA_C & 0x000000ffUL)     ); 

    record_packet[24] = ',';

    // SRV_W_CURR 
    record_packet[25] = (char)(((unsigned long)current_mA_D & 0xff000000UL) >> 24);
    record_packet[26] = (char)(((unsigned long)current_mA_D & 0x00ff0000UL) >> 16);
    record_packet[27] = (char)(((unsigned long)current_mA_D & 0x0000ff00UL) >> 8); 
    record_packet[28] = (char)(((unsigned long)current_mA_D & 0x000000ffUL)     ); 

    record_packet[29] = ',';

    // TOF
    // TODO(acauligi): what to do when ToF sensor reads faulty measurement?
    record_packet[30] = 0x00;
    record_packet[31] = 0x00;
    record_packet[32] = vl_range;
    
    record_packet[33] = ',';
    
    record_packet[34] = overtemperature_flag ? '*' : '-';
    
    // close the file:
    my_file.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening experiment file");
  }
}

// TODO(acauligi)
void ReadRecordFromCard() {
  /*
  // re-open the file for reading:
  String fn = String(experiment_idx + ".txt");
  my_file = SD.open(fn);
  if (my_file) {
    Serial.println("experiment file:");

    // read from the file until there's nothing else in it:
    while (my_file.available()) {
      Serial.write(my_file.read());
    }
    // close the file:
    my_file.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening experiment file");
  }
  */

  /* 
  int record_num = 0;
  String fn = String(experiment_idx + ".txt");
  my_file = SD.open(fn);
  if (my_file) {
    Serial.println("experiment file:");

    // read from the file until there's nothing else in it:
    while (my_file.available()) {
      String list = my_file.readStringUntil('\r');
      record_num++;
      if (record_num==record_number_of_interest) {
        // DO SOMETHING
      }
    }
    // close the file:
    my_file.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening experiment file");
  }
  */

  String fn = String(experiment_idx + ".txt");
  my_file = SD.open(fn);
  if (my_file) {
    my_file.seek(0);
    char cr;

    for (unsigned int i = 0; i < (record_num-1);) {
      cr = my_file.read();
      if (cr == '\n') {
        i++;
      }
    }

    // Now at correct line
    unsigned int exp_line_idx = 0;
    cr = my_file.read();
    while (true) {
      exp_line[exp_line_idx] = cr;
      cr = my_file.read();
      if (cr == '\n') {
        break;
      }
    }

    // close the file:
    my_file.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening experiment file");
  }
}
