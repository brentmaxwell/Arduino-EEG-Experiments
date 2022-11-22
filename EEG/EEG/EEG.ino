#define BAUD_RATE 115200
#define INPUT_PIN A0
#define SAMPLE_FREQ 128

#define SAMPLES 128         // must be power of 2

//TFT connection
#define TFT_DC 9
#define TFT_CS 10

//SD connection
#define SD_CS 4

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include <fix_fft.h>

const unsigned long sample_period = 1000000 / SAMPLE_FREQ;

char data[SAMPLES];
char im[SAMPLES];

unsigned long sample_timer;
unsigned long period_timer;

short screen = 0;
bool serialOutput = 0;
bool sdOutput = 1;

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.print("Initializing...");
  setupTft();
  setupSD();
  Serial.println("complete");
}

void loop() {
  static unsigned long sample_timer = micros() + sample_period;  // set loop()timer
  //static unsigned long period_timer = micros() + 1000000L;       // print results every second
  static short t = 0;

  checkTouch();

  while (micros() < sample_timer);    // wheel-spin until next sample due
  sample_timer += sample_period;      // reset loop timer

  if (t < SAMPLES) {
    int value = getData();
    data[t] = map(value, 0, 1023, 0, 255);
    im[t] = 0;
    String message = "R ";
    message += sample_timer;
    message += ":";
    message += value;
    message += "\n";
    if(serialOutput) {
      Serial.print(message);
    }
    writeFile(message);
    if(screen == 0){
      drawRawData(value);
    }
    t++;
  } else {
    processFFT();
    if(screen == 1){
      drawBinData();
    }
    t = 0;                              // reset data acquistition
    sample_timer = micros() + sample_period;  // reset loop() timer
  }
}
