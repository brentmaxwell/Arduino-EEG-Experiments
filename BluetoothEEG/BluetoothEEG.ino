#pragma GCC diagnostic ignored "-Wnarrowing"
#include <Adafruit_TinyUSB.h>
#include "Hardware.h"
#include "config.h"
#include "rtc.cpp"
#include "battery.cpp"
#include "statusLed.cpp"
#include "data.h"

Battery batt = Battery(BATTERY_PIN, ADC_RESOLUTION, ADC_VOLTAGE_REF);
//Battery batt = Battery();
Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(NUM_OF_NEOPIXELS, NEOPIXEL_PIN);
StatusLED statusLed = StatusLED(neopixel);
RtcClock rtcClock;

HeartRate heart_rate;

bool bleConnected = false;

unsigned long sample_timer;
unsigned long period_timer;
unsigned long startTimer = 0;
unsigned long timer = 0;
int timer_count = 0;  //multiply each timer by 4,294,967,295


int commandSource = 0;
String commandString = "";
bool stringComplete = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  batt.begin();
  statusLed.begin();
  Serial.begin(BAUD_RATE);
  delay(3000);
  if (digitalRead(BUTTON_PIN) == 1 || msc_mode) {
    msc_mode = true;
    setupMSC();
    statusLed.mscStatus(true);
    while (digitalRead(BUTTON_PIN) == 1) {
      statusLed.toggle();
      delay(200);
    }
    statusLed.on();
  } else {
    Serial.begin(BAUD_RATE);
    Serial.println("Initializing...");
    rtcClock.begin();
    setupSD();
    setupEEG();
    setupBLE();
    Serial.println("complete");
    Serial.flush();
  }
}

void loop() {
  static unsigned long secondTimer = micros();
  static bool buttonState;      // the current reading from the input pin
  static bool lastButtonState;  // the previous reading from the input pin
  static int press_count = 0;
  static unsigned long pressTimer;

  if (micros() < timer) timer_count++;
  timer = micros();
  loopCommand();
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState > lastButtonState) {
    pressTimer = timer;
    if (!msc_mode) {
      batt.update();
      double output[1] = { press_count };
      writeMessage(timer_count, timer, String(press_count));
      press_count++;
    }
  } else if (buttonState == 1 && (timer - pressTimer) > 3000000) {
    statusLed.off();
    NVIC_SystemReset();
  } else if (buttonState < lastButtonState) {
    pressTimer = 0;
  }
  lastButtonState = buttonState;
  if (!msc_mode) {
    loopEeg();
  }
  if (bleConnected) {
    loopBle();
    if (batt.uselc && timer - secondTimer > 10000000) {
      batt.update();
    } else if (timer - secondTimer > 600000000) {
      writeMessage(timer_count, timer, "Battery Update");
      batt.update();
      readEEG();
    }
  }
  if (timer - secondTimer > 1000000) {
    statusLed.toggle();
    secondTimer = timer;
  }
}
