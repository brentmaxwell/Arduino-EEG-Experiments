#include <SD.h>

File dataFile;
char filename[12];
bool cardAvailable = false;
  
void setupSD(){
  if(SD.begin(SD_CS)){
    cardAvailable = true;
    openFile();
  }
  else{
    Serial.println("SD Card initialization failure");
  }
}

void openFile(){
  if(cardAvailable){
    int fileNum = 1;

    do {
      fileNum++;
      sprintf(filename, "/eeg_%02d.txt", fileNum);
    } while (SD.exists(filename));
    Serial.print("Opening ");
    Serial.println(filename);
    dataFile = SD.open(filename, FILE_WRITE);
    if(dataFile){
      Serial.print(filename);
      Serial.println(" opened");
    }
    else{
      Serial.print("Error opening file ");
      Serial.println(filename);
    }
  }
}

void writeFile(String data){
  static short bufferSize = 0;
  if(dataFile){
    dataFile.print(data);
    bufferSize++;
    if(bufferSize == SAMPLES){
      dataFile.flush();
      bufferSize = 0;
    }

  }
  else {
    Serial.print("Error opening file ");
    Serial.println(filename);
  }
}