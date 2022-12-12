// #include <Adafruit_LittleFS.h>
// #include <InternalFileSystem.h>

// using namespace Adafruit_LittleFS_Namespace;

// #define CONFG_FILE "/eeg_config.ini"

// Adafruit_LittleFS_Namespace::File file(InternalFS);

// void loadConfig() {
//   InternalFS.begin();
//   file.open(CONFG_FILE, FILE_O_READ);

//   // file existed
//   if (file) {
//     uint32_t readlen;
//     char buffer[64] = { 0 };
//     readlen = file.read(buffer, sizeof(buffer));

//     buffer[readlen] = 0;
//     Serial.println(buffer);
//     file.close();
//   }
// }

// bool saveConfig(){
//   if (file.open(CONFG_FILE, FILE_O_WRITE)) {
//     file.write("");
//     file.close();
//     return true;
//   }
//   Serial.println("Failed!");
//   return false;
// }
