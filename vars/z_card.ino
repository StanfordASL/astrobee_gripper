void WriteToCard() {
  if(!file_is_open || !experiment_in_progress) {
    // TODO(acauligi): Send error byte?
    return;
  }
  UpdateGripperState(); 
  ConstructExperimentRecordLine(); 

  for (size_t k =0; k < record_packet_data_len; k++) {
    my_file.println(record_line[k]);
  }
  my_file.println("\r\n");    // TODO(acauligi): double check line ending
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
    unsigned int record_line_idx = 0;
    cr = my_file.read();
    while (true) {
      record_line[record_line_idx] = cr;
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
