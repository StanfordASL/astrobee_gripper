void WriteToCard() {
  if(!file_is_open || !experiment_in_progress) {
    // TODO(acauligi): Send error byte?
    return;
  }
  MeasureCurrentSensors(); 
  ConstructExperimentRecordLine(); 

  for (size_t k =0; k < record_packet_data_len; k++) {
    my_file.println(char(record_line[k]));
  }
  my_file.print("\r\n");    // TODO(acauligi): double check line ending
}

void ReadRecordFromCard() {
  if(!file_is_open) {
    return;
  }

  my_file.seek(0);
  char cr;

  unsigned int i;
  for (i = 1; i < (record_num);) {
    if (!my_file.available()) {
      break;
    }
    cr = my_file.read();
    if (cr == '\n') {
      i++;
    }
  }
  if (i != record_num-1) {
    // File has fewer lines than record_num
    my_file.close();
    return;
  }

  // Now at correct line
  unsigned int record_line_idx = 0;
  cr = my_file.read();

  while (true) {
  // for (size_t k =0; k < record_packet_data_len; k++) {
    if (!my_file.available()) {
      break;
    }
    // TODO(acauligi): issue with casting char cr to unsigned char record_line[k]
    record_line[k] = cr;
    cr = my_file.read();
    if (cr == '\n') {
      break;
    }
  }
}
