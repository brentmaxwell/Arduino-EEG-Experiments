void writeMessage(int timer_count, unsigned long timer, String messageData) {
  String message = rtcClock.getDateTime();
  message += ",";
  message += timer_count;
  message += ",";
  message += timer;
  message += ",M,";
  message += messageData;
  message += "\n";
  Serial.print(message);
  Serial.flush();

  //writeBle(message);
  writeFile(message);
}

void writeRawData(int timer_count, unsigned long timer, HeartRate heart_rate, double eeg_value) {
  String message = rtcClock.getDateTime();
  message += ",";
  message += timer_count;
  message += ",";
  message += timer;
  message += ",R,";
  message += heart_rate.value;
  message += ",";
  message += eeg_value;
  message += "\n";
  if (output_serial) {
    Serial.print(message);
    Serial.flush();
  }
  //writeBle(message);
  writeFile(message);
}


void writeBleRaw(HeartRate heart_rate, double eeg_value) {
  String message = "";
  message += heart_rate.value;
  message += ",";
  message += eeg_value;
  message += "\n";
  if (output_ble) {
    writeBle(message);
  }
}

void writeBleWaves(HeartRate heart_rate, double eeg_value, double waves[], int wave_len) {
  String message = "";
  message += heart_rate.value;
  message += ",";
  message += eeg_value;
  message += ",";
  for (char i = 0; i < wave_len; i++) {
    message += waves[i];
    message += ",";
  }
  message += "\n";
  if (output_ble) {
    writeBle(message);
  }
}

void writeData(String type, int timer_count, unsigned long timer, uint32_t messageData[], short sizeOf) {
  String message = rtcClock.getDateTime();
  message += ",";
  message += timer_count;
  message += ",";
  message += timer;
  message += ",";
  message += type;
  message += ",";
  for (int i = 0; i < sizeOf - 1; i++) {
    message += messageData[i];
    message += ",";
  }
  message += messageData[sizeOf - 1];
  message += ";\n";
  Serial.print(message);
  Serial.flush();
  //writeBle(message);
  //writeFile(message);
}
