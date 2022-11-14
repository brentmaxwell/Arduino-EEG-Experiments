/* FFT conversion of raw EEG data
 * Based off of code from https://www.instructables.com/Mind-Control-3-EEG-Monitor/
 * v0.0.1 - inital code, with LCD code removed
 * v0.0.2 - modified output to include micros()
 * v0.0.3 - update to comment structure and variable names
 */
#include "fix_fft.h"

#define ADC_INPUT A0
#define SAMPLE_FREQ 128
// must be power of 2
#define NUMBER_OF_DATA_POINTS 64

/* FFT */
const unsigned long nyquist_freq = SAMPLE_FREQ / 2;
// sample period in uS
const unsigned long sample_period = 1000000 / SAMPLE_FREQ;
// equivalent to m=log2(n) from which 2^m=n
const short m = (short(log10(NUMBER_OF_DATA_POINTS) / log10(2)));
char data[NUMBER_OF_DATA_POINTS];
char im[NUMBER_OF_DATA_POINTS];                                   // imaginary values
short dc_offset;
short dc_counter = 5;
bool data_valid = false;

unsigned long Timer1;
unsigned long Timer2;

void setup()
{
  Serial.begin(115200);

  // start timers
  Timer1 = micros() + sample_period;                        // set loop()timer
  Timer2 = micros() + 1000000L;                             // print results every second
}

void loop(void)
{
  static short i = 0;                                       // sample counter
  
  // loop() control
  while (micros() < Timer1);                                // wheel-spin until next sample due
  Timer1 += sample_period;                                  // reset loop timer

  if (i < NUMBER_OF_DATA_POINTS) {
    // Acquire 'n' samples 
    short value = analogRead(ADC_INPUT);
    data[i] = map(value, 0, 1023, 0, 255);
    im[i] = 0;
    i++;
  }
  else {
    // Output the data
    // once per second
    if (micros() > Timer2) {
      Timer2 += 1000000L;
      // process data
      fix_fft(data, im, m, 0);
      // calculate amplitudes, ignoring image data beyond n/2 (Nyquist)
      for (short i = 0; i < NUMBER_OF_DATA_POINTS / 2; i++) {                         
        data[i] = (sqrt(data[i] * data[i] + im[i] * im[i]));
      }

      // capture dc offset
      if (!data_valid && (dc_counter > 0)) {
        dc_counter--;
        if (dc_counter < 1) data_valid = true;
        dc_offset = data[0];
      }

      // output the data
      if (data_valid) {
        // remove dc offset
        data[0] -= dc_offset;
        // send all bins to Processing
        String message = String(micros()) + ":";
        for (short i = 0; i < 31; i++) {
          message += (int)data[i];
          message += ',';
        }
        message += (int)data[31];
        Serial.println(message);
      }
    }

    //  Prepare for next sample
    i = 0;                                                      // reset data acquistition
    Timer1 = micros() + sample_period;                          // reset loop() timer
  }
}

