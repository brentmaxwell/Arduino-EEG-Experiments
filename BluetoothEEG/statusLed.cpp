#include <Adafruit_NeoPixel.h>



class StatusLED {
private:
  Adafruit_NeoPixel _neopixel;
  byte _colors[3];
  bool _state;
  void setBlue(byte val) {
    this->_colors[2] = val;
    this->on();
  }
  void setGreen(byte val) {
    this->_colors[0] = val;
    this->on();
  }
  void setRed(byte val) {
    this->_colors[1] = val;
    this->on();
  }
public:
  StatusLED(Adafruit_NeoPixel &neopixel)
    : _neopixel(neopixel) {
  }
  void begin() {
    this->_neopixel.begin();
  }
  void setColor(uint8_t r, uint8_t g, uint8_t b) {
    this->_colors[0] = g;
    this->_colors[1] = r;
    this->_colors[2] = b;
    this->_neopixel.setPixelColor(0, _neopixel.Color(r, g, b));
    this->_neopixel.show();
    this->_state = true;
  }
  void off() {
    this->_neopixel.clear();
    this->_neopixel.show();
    this->_state = false;
  }
  void on() {
    this->_neopixel.setPixelColor(0, _neopixel.Color(this->_colors[0], this->_colors[1], this->_colors[2]));
    this->_neopixel.show();
    this->_state = true;
  }
  void toggle() {
    if (this->_state) {
      this->off();
    } else {
      this->on();
    }
  }
  void bluetoothStatus(bool status) {
    this->setBlue(status * 255);
  }
  void storageStatus(bool status) {
    this->setGreen(status * 255);
  }
  void mscStatus(bool status) {
    this->setRed(status * 255);
  }
};
