void ConstructExperimentRecordLine() {
  MeasureCurrentSensors();

  // TIME: running 16-bit unsigned counter since Teensy power-on
  cur_time_ms = millis();
  String TIME_ms = String(int(cur_time_ms));      // TODO(acauligi): convert to ms and ensure formatting
  TIME_ms.toCharArray((char*)record_line,5);

  record_line[5] = ',';

  record_line[6] = adhesive_engage ? 'E' : 'D';

  record_line[7] = wrist_lock ? 'L' : 'U';

  record_line[8] = automatic_mode_enable ? 'A' : '*';

  record_line[9] = ',';

  // SRV_L1_CURR
  String SRV_L1_CURR_mA = String(int(current_L1_mA));
  SRV_L1_CURR_mA.toCharArray((char*)record_line+10,4);

  record_line[14] = ',';

  // SRV_L2_CURR
  String SRV_L2_CURR_mA = String(int(current_L2_mA));
  SRV_L2_CURR_mA.toCharArray((char*)record_line+15,4);

  record_line[19] = ',';

  // SRV_R_CURR
  String SRV_R_CURR_mA = String(int(current_R_mA));
  SRV_R_CURR_mA.toCharArray((char*)record_line+20,4);

  record_line[24] = ',';

  // SRV_W_CURR
  String SRV_W_CURR_mA = String(int(current_W_mA));
  SRV_W_CURR_mA.toCharArray((char*)record_line+25,4);

  record_line[29] = ',';

  // TOF
  String TOF_mm = String(int(vl_range_mm));
  TOF_mm.toCharArray((char*)record_line+30,3);

  record_line[33] = ',';
  
  record_line[34] = overtemperature_flag ? '*' : '-';
}

void WriteToCard() {
  if(!file_is_open || !experiment_in_progress ||
   ((millis() - sd_card_last_write_time_ms) <= sd_card_write_delay_ms)) {
    // TODO(acauligi): Send error byte?
    return;
  }

  ConstructExperimentRecordLine(); 

  for (size_t k =0; k < record_packet_data_len; k++) {
    my_file.print(char(record_line[k]));
  }
  my_file.print("\r\n");    // TODO(acauligi): double check line ending
  sd_card_last_write_time_ms = millis();
}

void ReadRecordFromCard() {
  if(!file_is_open) {
    return;
  }

  if (!my_file.seek(record_num*(record_packet_data_len+2))) {
    err_state = ConstructErrorByte(ERR_SD);
    return;
  }

  char cr;
  for (size_t k =0; k < record_packet_data_len; k++) {
    if (!my_file.available()) break; 
    cr = my_file.read();
    record_line[k] = cr;    // TODO(acauligi): issue with casting char cr to unsigned char record_line[k]
  }
  record_num++;
}

