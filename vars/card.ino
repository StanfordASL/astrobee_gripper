void WriteToCard() {
  UpdateGripperState(); 

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  my_file = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (my_file) {
    size_t record_packet_len = 35; 
    unsigned char record_packet[record_packet_len];
    // TIME: running 16-bit unsigned counter since Teensy power-on
    time = millis();
    record_packet[0] = 0x00; 
    record_packet[1] = (time & 0xff000000UL) >> 24;
    record_packet[2] = (time & 0x00ff0000UL) >> 16;
    record_packet[3] = (time & 0x0000ff00UL) >> 8; 
    record_packet[4] = (time & 0x000000ffUL)     ; 

    record_packet[5] = ',';

    record_packet[6] = adhesive_engage ? 'E' : 'D';

    record_packet[7] = wrist_lock ? 'L' : 'U';

    record_packet[8] = automatic_mode_enable ? 'A' : '*';

    record_packet[9] = ',';

    // SRV_L1_CURR
    record_packet[10] = (current_mA_A & 0xff000000UL) >> 24;
    record_packet[11] = (current_mA_A & 0x00ff0000UL) >> 16;
    record_packet[12] = (current_mA_A & 0x0000ff00UL) >> 8; 
    record_packet[13] = (current_mA_A & 0x000000ffUL)     ;

    record_packet[14] = ',';

    // SRV_L2_CURR
    record_packet[15] = (current_mA_B & 0xff000000UL) >> 24;
    record_packet[16] = (current_mA_B & 0x00ff0000UL) >> 16;
    record_packet[17] = (current_mA_B & 0x0000ff00UL) >> 8; 
    record_packet[18] = (current_mA_B & 0x000000ffUL)     ; 

    record_packet[19] = ',';

    // SRV_R_CURR 
    record_packet[20] = (current_mA_C & 0xff000000UL) >> 24;
    record_packet[21] = (current_mA_C & 0x00ff0000UL) >> 16;
    record_packet[22] = (current_mA_C & 0x0000ff00UL) >> 8; 
    record_packet[23] = (current_mA_C & 0x000000ffUL)     ; 

    record_packet[24] = ',';

    // SRV_W_CURR 
    record_packet[25] = (current_mA_D & 0xff000000UL) >> 24;
    record_packet[26] = (current_mA_D & 0x00ff0000UL) >> 16;
    record_packet[27] = (current_mA_D & 0x0000ff00UL) >> 8; 
    record_packet[28] = (current_mA_D & 0x000000ffUL)     ; 

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
    Serial.println("error opening test.txt");
  }
}

// TODO(acauligi)
void ReadFromCard() {
  /*
  // re-open the file for reading:
  my_file = SD.open("test.txt");
  if (my_file) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (my_file.available()) {
      Serial.write(my_file.read());
    }
    // close the file:
    my_file.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  */
}
