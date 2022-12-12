#include <Adafruit_DotStar.h>

class StatusLED {
private:
  Adafruit_DotStar _dot;
  byte _colors[3];
  bool _state;
  void setBlue(byte val) {
    this->_colors[2] = val;
    this->setColor();
  }
  void setGreen(byte val) {
    this->_colors[0] = val;
    this->setColor();
  }
  void setRed(byte val) {
    this->_colors[1] = val;
    this->setColor();
  }
public:
  StatusLED(Adafruit_DotStar &dot)
    : _dot(dot) {
  }
  void begin() {
    this->_dot.begin();
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
  void set(uint8_t r, uint8_t g, uint8_t b) {
    this->_colors[0] = g;
    this->_colors[1] = r;
    this->_colors[2] = b;
    this->_dot.setPixelColor(0, r, g, b);
    this->_dot.show();
    this->_state = true;
  }
  void setColor() {
    this->_dot.setPixelColor(0, this->_colors[0], this->_colors[1], this->_colors[2]);
    this->_dot.show();
    this->_state = true;
  }
  void off() {
    this->_dot.clear();
    this->_dot.show();
    this->_state = false;
  }
  void on() {
    this->setColor();
  }
  void toggle() {
    if (this->_state) {
      this->off();
    } else {
      this->on();
    }
  }
};
