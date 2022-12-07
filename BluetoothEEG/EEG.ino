#define MAINS_FREQ 60
#define EEG_FREQ_LOW 0.5
#define EEG_FREQ_HIGH 60

#define DELTA       0
#define DELTA_LOW   (0.5)
#define DELTA_HIGH  (3.9)
#define THETA       1
#define THETA_LOW   (4.0)
#define THETA_HIGH  (7.9)
#define ALPHA       2
#define ALPHA_LOW   (8.0)
#define ALPHA_HIGH  (11.9)
#define BETA        3
#define BETA_LOW    (12.0)
#define BETA_HIGH   (29.9)
#define GAMMA       4
#define GAMMA_LOW   (30.0)
#define GAMMA_HIGH  (64.0)

const unsigned long sample_period = 1000000 / SAMPLE_FREQ;

bool notch = true;
bool highPass = true;
bool lowPass = true;

const double binWidth = SAMPLES / SAMPLE_FREQ;

int waveFreq[5][2] = {
  { 0, 4 },    // Delta
  { 4, 8 },    // Theta
  { 8, 12 },   // Alpha
  { 12, 30 },  // Beta
  { 30, 64 }   // Gamma
};

#include <arduinoFFT.h>
#include <KickFiltersRT.h>

KickFiltersRT<uint16_t> notchFilter;
KickFiltersRT<uint16_t> highPassFilter;
KickFiltersRT<uint16_t> lowPassFilter;

arduinoFFT FFT = arduinoFFT(eeg_filtered_data, eeg_imaginary_data, SAMPLES, SAMPLE_FREQ);

bool setupEEG() {
  analogReadResolution(ADC_RESOLUTION);
  delay(1);
  notchFilter.initnotch(0, 0, MAINS_FREQ, SAMPLE_FREQ);
  highPassFilter.inithighpass(0, EEG_FREQ_LOW, SAMPLE_FREQ);
  lowPassFilter.initlowpass(0, EEG_FREQ_HIGH, SAMPLE_FREQ);
  return true;
}

void loopEeg() {
  static unsigned long sample_timer = timer + sample_period;  // set loop()timer
  static short t = 0;

  if (timer >= sample_timer) {      // wheel-spin until next sample due
    sample_timer += sample_period;  // reset loop timer

    if (t < SAMPLES) {
      ble_out_counter++;
      eeg_raw_value = readEEG();
      ble_eeg_raw_output = double((eeg_filtered_value)*adc_conversion_factor * 50) + 30;
      eeg_filtered_value = filterEEG(eeg_raw_value);
      eeg_raw_data[t] = eeg_raw_value;  //notchFilter.notch(value);
      eeg_filtered_data[t] = eeg_filtered_value;
      eeg_imaginary_data[t] = 0;
      double output[3] = { heart_rate_value, eeg_raw_value, eeg_filtered_value };
      writeRawFile();
      writeRawSerial();
      if(ble_out_counter == 5){
        writeRawBle();
        ble_out_counter = 0;
      }
      t++;
    } else {
      processFFT();
      writeFFTFile();
      writeFFTSerial();
      //writeFFTBle();
      writeBleWaves();
      t = 0;                                 // reset data acquistition
      sample_timer = timer + sample_period;  // reset loop() timer
    }
  }
}

uint16_t readEEG() {
  return analogRead(EEG_PIN);
}

uint16_t filterEEG(uint16_t value) {
  uint16_t val2 = value;
  if (notch) val2 = notchFilter.notch(val2);
  if (lowPass) val2 = lowPassFilter.lowpass(val2);
  if (highPass) val2 = highPassFilter.highpass(val2);
  return val2;
}

void processFFT() {
  FFT.DCRemoval();                                          // Removes any DC offset from the recorded data
  FFT.Windowing(FFT_WIN_TYP_BLACKMAN_HARRIS, FFT_FORWARD);  // Sets the windowing for the FFT
  FFT.Compute(FFT_FORWARD);                                 // Computes the FFT
  FFT.ComplexToMagnitude();                                 // Determines magnitudes from imaginary data (we are not interested in the imaginary part for this FFT application)
  for (int i = 0; i < 5; i++) {
    waves[i] = round(averagePower(eeg_filtered_data, waveFreq[i][0] * binWidth, waveFreq[i][1] * binWidth));
    ble_wave_output[i] = double((waves[i]) * adc_conversion_factor * 20) + (4 * (i + 1));
  }
}

double averagePower(double realArray[], int start, int finish) {
  double sum;
  int coefficientCount = finish - start;
  for (start; start < finish; start++) {
    sum += (sq(realArray[start]));
  }
  return sqrt(sum / coefficientCount);
}

double getPeak() {
  return FFT.MajorPeak();
}