#pragma GCC diagnostic ignored "-Wnarrowing"

#include "Hardware.h"
#include "config.h"
#include <SPI.h>
#include <SoftwareSerial.h>

const double adc_conversion_factor = double(ADC_VOLTAGE_REF) / double(pow(2, ADC_RESOLUTION));


double eeg_raw_data[SAMPLES];
double eeg_filtered_data[SAMPLES];
double eeg_imaginary_data[SAMPLES];
double waves[5];

double eeg_raw_value;
double eeg_filtered_value;
bool hr_valid = false;
int heart_rate_value;

double ble_eeg_raw_output;
double ble_wave_output[5];

unsigned long sample_timer;
unsigned long period_timer;
unsigned long startTimer = 0;
unsigned long timer;
unsigned long pressTimer;

double battery_level = 0;
double battery_level_percent = 0;
bool buttonState;      // the current reading from the input pin
bool lastButtonState;  // the previous reading from the input pin
int press_count = 0;

byte ble_out_counter = 0;

String sensor_location = "";

void setup() {
  getBatteryLevel();
  Serial.begin(BAUD_RATE);
  delay(1000);
  Serial.println("Initializing...");
  Serial.print("Serial...");
  Serial.println("OK");
  Serial.flush();
  Serial.print("LED...");
  setupLED();
  Serial.println("OK");
  Serial.flush();
  Serial.print("EEG...");
  setupEEG();
  Serial.println("OK");
  Serial.flush();
  Serial.println("RTC...");
  if (setupRtc()) {
    Serial.println("OK");
  } else {
    Serial.println("ERROR");
  }
  Serial.flush();
  Serial.println("BLE...");
  if (setupBLE()) {
    Serial.println("OK");
  } else {
    Serial.println("ERROR");
  }
  Serial.flush();
  Serial.print("HRM...");
  if (setupHrm()) {
    Serial.println("OK");
  } else {
    Serial.println("ERROR");
  }
  Serial.flush();
  Serial.print("SD...");
  if (setupSD()) {
    Serial.println("OK");
  } else {
    Serial.println("ERROR");
  }
  Serial.flush();
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  Serial.println("complete");
}

void loop() {
  getBatteryLevel();
  timer = micros() - startTimer;
  //loopBle();
  if (Serial.available() > 0) {
    String testString = Serial.readString();
    writeMessage(timer, testString);
  }
  buttonState = digitalRead(PIN_BUTTON);
  if (buttonState < lastButtonState) {
    pressTimer = timer;
    press_count++;
    double output[1] = { press_count };
    writeData("M", timer, output, 1, MESSAGE);
  } else if (buttonState == 0 && (timer - pressTimer) > 3000000) {
    NVIC_SystemReset();
  } else if (buttonState > lastButtonState) {
    pressTimer = 0;
  }
  lastButtonState = buttonState;

  loopEeg();
}

// void getBatteryLevel(){
//   battery_level = (((double(analogRead(BATT_PIN)) * adc_conversion_factor)-3.3)/(ADC_VOLTAGE_REF-3.3)) * 100;
// }
