#define NUMPIXELS 1

#include <Adafruit_DotStar.h>

byte color[3];

Adafruit_DotStar dot(NUMPIXELS, DOTSTAR_DATAPIN, DOTSTAR_CLOCKPIN, DOTSTAR_BRG);

bool setupLED() {
  dot.begin();
  setLed(0, 0, 0);
  return true;
}

void setLed(uint8_t r, uint8_t g, uint8_t b) {
  color[0] = g;
  color[1] = r;
  color[2] = b;
  dot.setPixelColor(0, r, g, b);
  dot.show();
}

void setColor() {
  dot.setPixelColor(0, color[0], color[1], color[2]);
  dot.show();
}

void setRed(byte val) {
  color[1] = val;
  setColor();
}

void setGreen(byte val) {
  color[0] = val;
  setColor();
}

void setBlue(byte val) {
  color[2] = val;
  setColor();
}
