#pragma GCC diagnostic ignored "-Wnarrowing"
#include "Hardware.h"
#include "config.h"
#include "rtc.cpp"
#include "battery.cpp"
#include "led.cpp"
#include "data.h"

Battery batt(BATTERY_PIN, ADC_RESOLUTION, ADC_VOLTAGE_REF);
Adafruit_DotStar dot(DOTSTAR_NUMPIXELS, DOTSTAR_DATAPIN, DOTSTAR_CLOCKPIN, DOTSTAR_BRG);
StatusLED statusLed(dot);
RtcClock rtcClock;

HeartRate heart_rate;

unsigned long sample_timer;
unsigned long period_timer;
unsigned long startTimer = 0;
unsigned long timer = 0;
int timer_count = 0;  //multiply each timer by 4,294,967,295


int commandSource = 0;
String commandString = "";
bool stringComplete = false;

void setup() {
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_USB_DETECT, INPUT_PULLDOWN);
  statusLed.begin();
  Serial.begin(BAUD_RATE);
  delay(3000);
  if (digitalRead(PIN_BUTTON) == 0 || msc_mode) {
    msc_mode = true;
    setupMSC();
    statusLed.mscStatus(true);
    while (digitalRead(PIN_BUTTON) == 0) {
      statusLed.toggle();
      delay(200);
    }
    statusLed.on();
    Serial.println("Connected Mass Storage");
  } else {
    Serial.begin(BAUD_RATE);
    Serial.println("Initializing...");
    rtcClock.begin();
    setupSD();
    setupEEG();
    setupBLE();
    setupHrm();
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
  if (timer - secondTimer > 1000000) {
    batt.update();
    statusLed.toggle();
    secondTimer = timer;
  }
  loopCommand();
  buttonState = digitalRead(PIN_BUTTON);
  if (buttonState < lastButtonState) {
    pressTimer = timer;
    if (!msc_mode) {
      double output[1] = { press_count };
      writeMessage(timer_count, timer, String(press_count));
      press_count++;
    }
  } else if (buttonState == 0 && (timer - pressTimer) > 3000000) {
    NVIC_SystemReset();
  } else if (buttonState > lastButtonState) {
    pressTimer = 0;
  }
  lastButtonState = buttonState;
  if (!msc_mode) {
    loopBle();
    loopEeg();
  }
}
