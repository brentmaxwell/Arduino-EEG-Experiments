void writeData(String type, unsigned long timer, double messageData[], short sizeOf, byte source) {
  String message = "";
  message += timer;
  message += ",";
  message += type;
  message += ",";
  for (byte i = 0; i < sizeOf - 1; i++) {
    message += messageData[i];
    message += ",";
  }
  message += messageData[sizeOf - 1];
  message += ";\n";
  Serial.print(getTime());
  Serial.print(" ");
  Serial.print(battery_level_percent);
  Serial.print(" ");
  Serial.print(message);
  Serial.flush();
  writeBle(message);
  writeFile(message);
}

void writeMessage(unsigned long timer, String messageData) {
  String message = "";
  message += timer;
  message += ",M,";
  message += messageData;
  Serial.print(message);
  Serial.flush();
  writeBle(message);
  writeFile(message);
}


void writeRawSerial() {
  String message = "";
  message += timer;
  message += ",R,";
  message += heart_rate_value;
  message += ",";
  message += eeg_raw_value;
  message += ",";
  message += eeg_filtered_value;
  message += ",\n";
  Serial.print(getTime());
  Serial.print(" ");
  Serial.print(battery_level_percent);
  Serial.print(" ");
  Serial.print(message);
  Serial.flush();
}

void writeWavesSerial() {
  String message = "";
  message += ",,";
  for (char i = 0; i < 5; i++) {
    message += waves[i];
    message += ",";
  }
  message += "\n";
  Serial.print(message);
  Serial.flush();
}

void writeRawFile() {
  String message = "";
  message += timer;
  message += ",R,";
  message += heart_rate_value;
  message += ",";
  message += eeg_raw_value;
  message += ",";
  message += eeg_filtered_value;
  message += ",\n";
  writeFile(message);
}

void writeRawBle() {
  String message = "";
  if (hr_valid) message += heart_rate_value;
  message += ",";
  message += ble_eeg_raw_output;
  message += "\n";
  writeBle(message);
}

void writeFFTSerial() {
  String message = "";
  message += timer;
  message += ",F,,,,";
  for (byte i = 0; i < SAMPLES - 1; i++) {
    message += eeg_filtered_data[i];
    message += ",";
  }
  message += eeg_filtered_data[SAMPLES - 1];
  message += "\n";
  Serial.print(message);
  Serial.flush();
}

void writeFFTFile() {
  String message = "";
  message += timer;
  message += ",F,,,,";
  for (byte i = 0; i < SAMPLES - 1; i++) {
    message += eeg_filtered_data[i];
    message += ",";
  }
  message += eeg_filtered_data[SAMPLES - 1];
  message += "\n";
  writeFile(message);
}

void writeFFTBle() {
  String message = "";
  message += timer;
  message += ",,,,";
  for (int i = 0; i < SAMPLES - 1; i++) {
    message += eeg_filtered_data[i];
    message += ",";
  }
  message += eeg_filtered_data[SAMPLES - 1];
  message += "\n";
  writeBle(message);
}

void writeBleWaves() {
   String message = "";
  if (hr_valid) message += heart_rate_value;
  message += ",";
  message += ble_eeg_raw_output;
  message += ",";
  for (char i = 0; i < 4; i++) {
    message += ble_wave_output[i];
    message += ",";
  }
  message += "\n";
  writeBle(message);
}