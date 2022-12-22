// #define SAMPLE_FREQ 512
// #define SAMPLES 512  // must be power of 2
// #define MAINS_FREQ 60
// #define EEG_FREQ_LOW 0.5
// #define EEG_FREQ_HIGH 64

// #define DELTA 0
// #define DELTA_LOW (0.5)
// #define DELTA_HIGH (3.9)
// #define THETA 1
// #define THETA_LOW (4.0)
// #define THETA_HIGH (7.9)
// #define ALPHA 2
// #define ALPHA_LOW (8.0)
// #define ALPHA_HIGH (11.9)
// #define BETA 3
// #define BETA_LOW (12.0)
// #define BETA_HIGH (29.9)
// #define GAMMA 4
// #define GAMMA_LOW (30.0)
// #define GAMMA_HIGH (64.0)

// #include <arduinoFFT.h>
// #include <KickFiltersRT.h>
// double waveFreq[5][2] = {
//   { DELTA_LOW, DELTA_HIGH },  // Delta
//   { THETA_LOW, THETA_HIGH },  // Theta
//   { ALPHA_LOW, ALPHA_HIGH },  // Alpha
//   { BETA_LOW, BETA_HIGH },    // Beta
//   { GAMMA_LOW, GAMMA_HIGH }   // Gamma
// };

// class EEG{
//   private:
//     uint8_t _pin;
//     int _value;
//     double _imaginary_data[SAMPLES];
//     KickFiltersRT<uint16_t> _notchFilter;
//     KickFiltersRT<uint16_t> _highPassFilter;
//     KickFiltersRT<uint16_t> _lowPassFilter;
//     arduinoFFT _FFT;
//     double adc_conversion_factor;
//     int bin_width;
//   public:
//     double raw_data[SAMPLES];
//     double filtered_data[SAMPLES];
//     double waves[5];
//     double ble_raw;
//     double ble_wave_output[5];
//     EEG(uint8_t pin, int sample_freq, int samples){
//       this->adc_conversion_factor = double(3.6) / double(pow(2, 14));
//       this->_pin = pin;
//       this->_FFT = arduinoFFT(this->filtered_data, this->_imaginary_data, samples, sample_freq);
//       this->_notchFilter.initnotch(0, 0, MAINS_FREQ, SAMPLE_FREQ);
//       this->_highPassFilter.inithighpass(0, EEG_FREQ_LOW, SAMPLE_FREQ);
//       this->_lowPassFilter.initlowpass(0, EEG_FREQ_HIGH, SAMPLE_FREQ);
//       this->bin_width = SAMPLES / SAMPLE_FREQ;
//     }
//     int16_t read(){
//       analogRead(_pin);
//       this->_value = analogRead(_pin);
//       return this->_value;
//     }
//     int16_t filter(uint16_t value) {
//       return this->_highPassFilter.highpass(this->_lowPassFilter.lowpass(this->_notchFilter.notch(value)));
//     }
//     void processFFT(){
//       this->_FFT.DCRemoval();                                          // Removes any DC offset from the recorded data
//       this->_FFT.Windowing(FFT_WIN_TYP_BLACKMAN_HARRIS, FFT_FORWARD);  // Sets the windowing for the FFT
//       this->_FFT.Compute(FFT_FORWARD);                                 // Computes the FFT
//       this->_FFT.ComplexToMagnitude();                                 // Determines magnitudes from imaginary data (we are not interested in the imaginary part for this FFT application)
//       for (byte i = 0; i < 5; i++) {
//         this->waves[i] = getWaveValue(this->filtered_data, i);
//         this->ble_wave_output[i] = double((this->waves[i]) * this->adc_conversion_factor) + (2 * (i + 1));
//       }
//     }  
//     double averagePower(double realArray[], int start, int finish) {
//       double sum;
//       int coefficientCount = finish - start;
//       for (start; start < finish; start++) {
//         sum += (sq(realArray[start]));
//       }
//       return sqrt(sum / coefficientCount);
//     }

//     double getWaveValue(double data[], byte wave_type) {
//       return round(this->averagePower(data, floor(double(waveFreq[wave_type][0] * this->bin_width)), ceil(waveFreq[wave_type][1] * this->bin_width)));
//     }
// };