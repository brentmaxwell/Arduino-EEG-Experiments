#define BAUD_RATE 115200
#define SAMPLE_FREQ 128
#define SAMPLES 128  // must be power of 2

#define EEG_RAW 0
#define EEG_PROCESSED 1
#define HRM_OUTPUT 2
#define MESSAGE 3

#define OUTPUT_SERIAL 0
#define OUTPUT_BLE 1
#define OUTPUT_FILE 2

bool outputConfig[4][3] = {
  //serial, ble, file
  { true, false, true },  // eeg raw
  { true, false, true },  // eeg processed
  { true, false, true },  // hrm
  { true, false, true }   // message
};

bool writeToFile = true;
bool AndroidBleOutput = false;