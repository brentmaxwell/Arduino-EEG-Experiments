#include <RTClib.h>

class RtcClock {
private:
  RTC_PCF8523 _rtc;
public:
  bool begin() {
    if (this->_rtc.begin()) {
      if (!this->_rtc.initialized() || this->_rtc.lostPower()) {
        Serial.println("RTC is NOT initialized, let's set the time!");
        Serial.flush();
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        // delay 2 seconds to let the osillator settle
        delay(2000);
        this->_rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      }
      this->_rtc.start();
      return true;
    }
    return false;
  }
  String getDateTime() {
    if (this->_rtc.initialized() && this->_rtc.isrunning()) {
      DateTime now = this->_rtc.now();
      char dateTime[29];
      sprintf(dateTime, "%04d/%02d/%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
      return String(dateTime);
    } else {
      return String(micros());
    }
  }
  void setDateTime(int year, int month, int day, int hour, int minute, int second) {
    this->_rtc.adjust(DateTime(year, month, day, hour, minute, second));
  }
  DateTime now() {
    return this->_rtc.now();
  }
};
