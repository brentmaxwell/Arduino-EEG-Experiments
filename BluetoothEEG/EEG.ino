#define MAINS_FREQ 60
#define EEG_FREQ_LOW 0.5
#define EEG_FREQ_HIGH 64

#define DELTA 0
#define DELTA_LOW (0.5)
#define DELTA_HIGH (3.9)
#define THETA 1
#define THETA_LOW (4.0)
#define THETA_HIGH (7.9)
#define ALPHA 2
#define ALPHA_LOW (8.0)
#define ALPHA_HIGH (11.9)
#define BETA 3
#define BETA_LOW (12.0)
#define BETA_HIGH (29.9)
#define GAMMA 4
#define GAMMA_LOW (30.0)
#define GAMMA_HIGH (64.0)

#include <arduinoFFT.h>
#include <KickFiltersRT.h>

const unsigned long sample_period = 1000000 / SAMPLE_FREQ;

const double binWidth = SAMPLES / SAMPLE_FREQ;

int waveFreq[5][2] = {
  { DELTA_LOW, DELTA_HIGH },  // Delta
  { THETA_LOW, THETA_HIGH },  // Theta
  { ALPHA_LOW, ALPHA_HIGH },  // Alpha
  { BETA_LOW, BETA_HIGH },    // Beta
  { GAMMA_LOW, GAMMA_HIGH }   // Gamma
};

double eeg_raw_data[SAMPLES];
double eeg_filtered_data[SAMPLES];
double eeg_imaginary_data[SAMPLES];
double waves[5];

double eeg_raw;
double eeg_filtered;

double ble_eeg_raw;
double ble_wave_output[5];

byte ble_out_counter = 0;

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

  if (sample_timer - timer >= sample_period) {
    sample_timer += sample_period;  // reset loop timer

    if (t < SAMPLES) {
      ble_out_counter++;
      eeg_raw = readEEG();
      eeg_filtered = filterEEG(eeg_raw);
      ble_eeg_raw = double((eeg_filtered)*adc_conversion_factor * 50) + 30;
      eeg_raw_data[t] = eeg_raw;
      eeg_filtered_data[t] = eeg_filtered;
      eeg_imaginary_data[t] = 0;
      writeRawData(timer_count, timer, heart_rate, eeg_raw);
      if (ble_out_counter == 5) {
        writeBleRaw(heart_rate, ble_eeg_raw);
        ble_out_counter = 0;
      }
      t++;
    } else {
      processFFT();
      writeBleWaves(heart_rate, ble_eeg_raw, ble_wave_output, 5);
      t = 0;                                 // reset data acquistition
      sample_timer = timer + sample_period;  // reset loop() timer
    }
  }
}

uint16_t readEEG() {
  return analogRead(EEG_PIN);
}

uint16_t filterEEG(uint16_t value) {
  return highPassFilter.highpass(lowPassFilter.lowpass(notchFilter.notch(value)));
}

void processFFT() {
  FFT.DCRemoval();                                          // Removes any DC offset from the recorded data
  FFT.Windowing(FFT_WIN_TYP_BLACKMAN_HARRIS, FFT_FORWARD);  // Sets the windowing for the FFT
  FFT.Compute(FFT_FORWARD);                                 // Computes the FFT
  FFT.ComplexToMagnitude();                                 // Determines magnitudes from imaginary data (we are not interested in the imaginary part for this FFT application)
  for (byte i = 0; i < 5; i++) {
    waves[i] = getWaveValue(eeg_filtered_data, i);
    ble_wave_output[i] = double((waves[i]) * adc_conversion_factor * 20);  // + (2 * (i + 1));
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

double getWaveValue(double data[], byte wave_type) {
  return round(averagePower(data, floor(double(waveFreq[wave_type][0] * binWidth)), ceil(waveFreq[wave_type][1] * binWidth)));
}

double getPeak() {
  return FFT.MajorPeak();
}