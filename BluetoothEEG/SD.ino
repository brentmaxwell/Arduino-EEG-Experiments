#include <SPI.h>
#include <SdFat.h>

SdFat SD;
SdFile root;
SdFile dataFile;
Adafruit_USBD_MSC usb_msc;

//char filename[34];
bool cardAvailable = false;

bool setupSD() {
  Serial.print("SD...");
  if (SD.begin(SD_CS)) {
    cardAvailable = true;
    Serial.println("OK");
    Serial.flush();
    statusLed.storageStatus(true);
    SdFile::dateTimeCallback(fileDateTime);
    return openFile();
  } else {
    Serial.println("ERROR");
    statusLed.storageStatus(false);
    return false;
  }
}

void fileDateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtcClock.now();
  *date = FAT_DATE(now.year(), now.month(), now.day());
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}

void getFilename(char* buf) {
  DateTime now = rtcClock.now();
  sprintf(buf, "%04d-%02d-%02dT%02d-%02d-%02d.txt", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
}

bool openFile() {
  if (cardAvailable) {
    int fileNum = 0;
    //do {
    //  fileNum++;
    //sprintf(filename, "/eeg_%02d.txt", fileNum);
    //} while (SD.exists(filename));

    char filename[23];
    getFilename(filename);
    Serial.print("Opening ");
    Serial.print(filename);
    Serial.println("...");
    dataFile.open(filename, FILE_WRITE);
    if (dataFile) {
      dataFile.print("DateTime: ");
      dataFile.println(rtcClock.getDateTime());
      dataFile.print("Sample Frequency: ");
      dataFile.println(SAMPLE_FREQ);
      dataFile.print("Samples/Period: ");
      dataFile.println(SAMPLES);
      dataFile.println("-------------------------------------------");
      dataFile.println("Time,Timer Count,Timer,Heart Rate,EEG Raw");
      dataFile.flush();
      Serial.println("OK");
      Serial.flush();
      statusLed.storageStatus(true);
      return true;
    } else {
      Serial.println("ERROR");
      Serial.flush();
      statusLed.storageStatus(false);
      return false;
    }
  }
  statusLed.storageStatus(false);
  return false;
}



void writeFile(String data) {
  static short bufferSize = 0;
  if (dataFile && output_file) {
    dataFile.print(data);
    bufferSize++;
    if (bufferSize == SAMPLES * 2) {
      dataFile.flush();
      bufferSize = 0;
      statusLed.storageStatus(true);
    }
  } else {
    statusLed.storageStatus(false);
  }
}

String listFiles() {
  DirFat_t dir;
  String fileList;
  SdFile file;
  root.open("/");
  while (file.openNext(&root, O_RDONLY)) {
    String fileRow;
    char fileName[255];

    file.dirEntry(&dir);
    file.getName(fileName, 255);
    fileRow = String(fileName);
    if (file.isDir()) {
      fileRow += "/";
    }
    for (char i = fileRow.length(); i < 50; i++) {
      fileRow += " ";
    }
    if (!file.isDir()) {
      fileRow += file.fileSize();
    }
    for (char i = fileRow.length(); i < 65; i++) {
      fileRow += " ";
    }
    fileRow += fileDateTime(dir.createDate, dir.createTime).timestamp();
    fileRow += "\n";
    fileList += fileRow;
    file.close();
  }
  root.close();
  return fileList;
}

bool deleteFile(String filename) {
  return SD.remove(filename);
}


DateTime fileDateTime(uint8_t createDate_arr[], uint8_t createTime_arr[]) {
  int createDate = (createDate_arr[1] << 8) + createDate_arr[0];
  int createTime = (createTime_arr[1] << 8) + createTime_arr[0];
  return DateTime(FS_YEAR(createDate), FS_MONTH(createDate), FS_DAY(createDate), FS_HOUR(createTime), FS_MINUTE(createTime), FS_SECOND(createTime));
}

// the setup function runs once when you press reset or power the board
void setupMSC() {
  Serial.println("Mass Storage Device...");
  // Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
  usb_msc.setID("Adafruit", "SD Card", "1.0");

  // Set read write callback
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);

  // Still initialize MSC but tell usb stack that MSC is not ready to read/write
  // If we don't initialize, board will be enumerated as CDC only
  usb_msc.setUnitReady(false);
  usb_msc.begin();

  Serial.begin(BAUD_RATE);

  Serial.println("\nInitializing SD card...");

  if (!SD.begin(SD_CS, SD_SCK_MHZ(50))) {
    Serial.println("ERROR");
    statusLed.off();
    return;
  }

  uint32_t block_count = SD.card()->cardSize();

  Serial.print("Volume size (MB):  ");
  Serial.println((block_count / 2) / 1024);

  // Set disk size, SD block size is always 512
  usb_msc.setCapacity(block_count, 512);

  // MSC is ready for read/write
  usb_msc.setUnitReady(true);
  Serial.println("Connected Mass Storage");
  Serial.println("OK");
  Serial.flush();
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and
// return number of copied bytes (must be multiple of block size)
int32_t msc_read_cb(uint32_t lba, void* buffer, uint32_t bufsize) {
  (void)bufsize;
  return SD.card()->readBlock(lba, (uint8_t*)buffer) ? 512 : -1;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and
// return number of written bytes (must be multiple of block size)
int32_t msc_write_cb(uint32_t lba, uint8_t* buffer, uint32_t bufsize) {
  (void)bufsize;
  return SD.card()->writeBlock(lba, buffer) ? 512 : -1;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void msc_flush_cb(void) {
  SD.card()->syncBlocks();

  // clear file system's cache to force refresh
  SD.cacheClear();
}
