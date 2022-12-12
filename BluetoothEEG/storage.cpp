// #include "SPI.h"
// #include <SdFat.h>

// class Storage{
//   private:
//     unsigned char _chip_select;
//     unsigned int _sample_frequency;
//     unsigned int _samples;
//     SdFat _sd;
//     SdFile _datafile;
//     char _filename[12];
//     bool _card_available;
//     int _bufferSize = 0;
//     // static void _fileDateTime(uint16_t* date, uint16_t* time) {
//     //   DateTime now = _rtcClock.now();
//     //   *date = FAT_DATE(now.year(), now.month(), now.day());
//     //   *time = FAT_TIME(now.hour(), now.minute(), now.second());
//     // }
//   public:
//       bool status;
//       Storage(unsigned char chip_select, unsigned int sample_frequency, unsigned int samples){
//         this->_chip_select = chip_select;
//         this->_sample_frequency = sample_frequency;
//         this->_samples = samples;
//       }
//       bool begin(){
//         if (this->_sd.begin(this->_chip_select)) {
//           this->_card_available = true;
//           //SdFile::dateTimeCallback(Storage::_fileDateTime);
//           this->status = true;
//           return this->openFile();
//         } else {
//           this->status = false;
//           return false;
//         }
//       }
//       bool openFile() {
//         if (this->_card_available) {
//           int fileNum = 0;
//           do {
//             fileNum++;
//             sprintf(this->_filename, "/eeg_%02d.txt", fileNum);
//           } while (this->_sd.exists(this->_filename));

//           this->_datafile.open(this->_filename, FILE_WRITE);
//           if (this->_datafile) {
//             this->_datafile.print("DateTime: ");
//             //this->_datafile.println(rtcClock.getDateTime());
//             this->_datafile.print("Sample Frequency: ");
//             this->_datafile.println(this->_sample_frequency);
//             this->_datafile.print("Samples/Period: ");
//             this->_datafile.println(this->_samples);
//             this->_datafile.println("-------------------------------------------");
//             this->_datafile.println("Time,Timer Count,Timer,Heart Rate,EEG Raw");
//             this->_datafile.flush();
//             this->status = true;
//             return true;
//           } else {
//             this->status = false;
//             return false;
//           }
//         }
//         this->status = false;
//         return false;
//       }
//       void write(String data) {
//         if (this->_datafile) {
//           this->_datafile.print(data);
//           this->_bufferSize++;
//           if (this->_bufferSize == this->_samples * 2) {
//             this->_datafile.flush();
//             this->_bufferSize = 0;
//             this->status = true;
//           }
//         } else {
//           this->status = false;
//         }
//       }

// };