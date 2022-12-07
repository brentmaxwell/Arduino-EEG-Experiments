#include <SD.h>

Adafruit_USBD_MSC usb_msc;

Sd2Card card;
SdVolume volume;

File dataFile;
char filename[12];
bool cardAvailable = false;

bool setupSD() {
  if (SD.begin(SD_CS)) {
    cardAvailable = true;
    setRed(255);
    return openFile();
  } else {
    Serial.println("SD Card initialization failure");
    setRed(0);
    return false;
  }
}

bool openFile() {
  if (cardAvailable) {
    int fileNum = 0;
    do {
      fileNum++;
      sprintf(filename, "/eeg_%02d.txt", fileNum);
    } while (SD.exists(filename));
    Serial.print("Opening ");
    Serial.println(filename);
    dataFile = SD.open(filename, FILE_WRITE);
    if (dataFile) {
      Serial.print(filename);
      dataFile.print("DateTime: ");
      dataFile.println(getTime());
      dataFile.print("Sample Frequency: ");
      dataFile.println(SAMPLE_FREQ);
      dataFile.print("Samples/Period: ");
      dataFile.println(SAMPLES);
      dataFile.println("----------------------------------------------------------------");
      dataFile.print("Timer,Heart Rate,EEG Raw,EEG Filtered,");
      for (int i = 0; i < SAMPLES - 1; i++) {
        dataFile.print("Sample ");
        dataFile.print(i);
        dataFile.print(",");
      }
      dataFile.print("Sample ");
      dataFile.println(SAMPLES - 1);
      dataFile.flush();
      startTimer = micros();
      Serial.println(" opened");
      setRed(255);
      return true;
    } else {
      Serial.print("Error opening file ");
      Serial.println(filename);
      setRed(0);
      return false;
    }
  }
  return false;
}

void writeFile(String data) {
  static short bufferSize = 0;
  if (dataFile && writeToFile) {
    dataFile.print(data);
    bufferSize++;
    if (bufferSize == SAMPLES * 2) {
      dataFile.flush();
      bufferSize = 0;
      setRed(255);
    }
    setRed(255);
  } else {
    setRed(0);
  }
}
