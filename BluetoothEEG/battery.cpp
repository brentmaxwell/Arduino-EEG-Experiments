#include <Arduino.h>

class Battery {
private:
  byte _pin;
  double _analog_resolution;
  double _mv_reference;
  float _vbat_divider;
  double _vbat_mv_per_lsb;
  double _real_vbat_mv_per_lsb;
  float _level;
  float _percent;
public:
  Battery(byte pin, byte analog_resolution, double v_reference) {
    this->_pin = pin;
    this->_mv_reference = v_reference * 1000.0F;
    this->_analog_resolution = pow(2, analog_resolution);
    this->_vbat_mv_per_lsb = (this->_mv_reference) / (this->_analog_resolution);
    this->_real_vbat_mv_per_lsb = 2.0F * this->_vbat_mv_per_lsb;
  }
  void update() {
    // Convert the raw value to compensated mv, taking the resistor-
    // divider into account (providing the actual LIPO voltage)
    // ADC range is 0..3000mV and resolution is 12-bit (0..4095)
    analogRead(_pin); //throw it out to allow the multiplexer to reset
    this->_level =  analogRead(_pin) * this->_real_vbat_mv_per_lsb;
    if (this->_level < 3300)
      this->_percent = 0;

    if (this->_level < 3600) {
      this->_percent = (this->_level - 3300) / 30;
    } else {
      this->_percent = 100;  // thats mvolts /6.66666666
    }
  }
  double getBatteryLevel() {
    return this->_level;
  }
  double getBatteryPercent() {
    return this->_percent;
  }
};
