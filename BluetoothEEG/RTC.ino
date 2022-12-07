#include <RTClib.h>

RTC_PCF8523 rtc;

bool setupRtc() {
  if (rtc.begin()) {
    if (!rtc.initialized() || rtc.lostPower()) {
      Serial.println("RTC is NOT initialized, let's set the time!");
      Serial.flush();
      // When time needs to be set on a new device, or after a power loss, the
      // following line sets the RTC to the date & time this sketch was compiled
      // delay 2 seconds to let the osillator settle
      delay(2000);
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    rtc.start();

    return true;
  }
  Serial.println("RTC ERROR");
  return false;
}

String getTime() {
  if (rtc.initialized()) {
    DateTime now = rtc.now();
    char dateTime[29];
    sprintf(dateTime, "%04d/%02d/%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
    return String(dateTime);
  } else {
    return String(micros());
  }
}

void setTime(int year, int month, int day, int hour, int minute, int second) {
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
}
