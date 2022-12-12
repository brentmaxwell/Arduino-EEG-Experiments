#define BAUD_RATE 115200
#define SAMPLE_FREQ 256
#define SAMPLES 256  // must be power of 2

#define EEG_RAW 0
#define EEG_PROCESSED 1
#define HRM_OUTPUT 2
#define MESSAGE 3

#define OUTPUT_SERIAL 0
#define OUTPUT_BLE 1
#define OUTPUT_FILE 2

#define BTLE_NAME "theBrent EEG"
#define BTLE_MANUFACTURER "theBrent"
#define BTLE_MODEL "EEG Headband v0.9"

const double adc_conversion_factor = double(ADC_VOLTAGE_REF) / double(pow(2, ADC_RESOLUTION));

bool outputConfig[4][3] = {
  //serial, ble, file
  { true, false, true },  // eeg raw
  { true, false, true },  // eeg processed
  { true, false, true },  // hrm
  { true, false, true }   // message
};

bool output_serial = true;
bool output_ble = true;
bool output_file = true;

bool msc_mode = false;
