#define TS_THRESHOLD 128
#define BUTTON_WIDTH 30
#define BUTTON_HEIGHT 30

#define BUTTON_Y1 290
#define BUTTON_Y2 320
#define BUTTON_X1 210
#define BUTTON_X2 240

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_FT6206 ts = Adafruit_FT6206();

const short TIME_PERIODS = SAMPLE_FREQ*10;
const short TIME_PER_PIXEL = SAMPLE_FREQ/32;
const short ZERO_OFFSET = map(420,0,1023,0,220)*2;

uint16_t colors[5] = { ILI9341_WHITE, ILI9341_YELLOW, ILI9341_RED, ILI9341_PURPLE, ILI9341_BLUE };

char bins[5];
short binWidth = SAMPLE_FREQ / SAMPLES;
char binFreq[5][2] = {
  {1, 4},
  {4, 8},
  {8, 16},
  {16, 32},
  {32, 64}
};

short x;
long time = 0;


void setupTft() {
  tft.cp437(true);
  tft.begin();
  ts.begin(TS_THRESHOLD);
  drawScreen();
}

void checkTouch(){
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    if (p.x > BUTTON_X1 && p.x < BUTTON_X2 && p.y > BUTTON_Y1 && p.y < BUTTON_Y2) {
      screen = abs(screen - 1);
      drawScreen();
    }
  }
}

void drawScreen(){
  time = micros();
  tft.setRotation(3);               // landscape
  tft.fillScreen(ILI9341_BLACK);    // black background
  tft.setTextColor(ILI9341_WHITE);  // white text
  
  tft.drawRect(290, 0, 30, 30, ILI9341_WHITE);
  tft.setCursor(294, 10);
  tft.println("Mode");
  // tft.drawRect(290, 30, 30, 30, ILI9341_WHITE);
  // tft.setCursor(294, 40);
  // tft.println("Out");
  // tft.drawRect(290, 60, 30, 30, ILI9341_WHITE);
  // tft.setCursor(294, 70);
  // tft.println("File");

  switch(screen){
    case 0:
      drawScreen0();
      break;
    case 1:
      drawScreen1();
      break;
  }
}

void drawScreen0(){
  x = 0;
  tft.drawFastVLine(0, 0, 220, ILI9341_GREEN);
  tft.drawFastHLine(0, 220, 320, ILI9341_GREEN);
  for (short i = 0; i <= 320; i += 32) {
    tft.drawFastVLine(0 + i, 220, 5, ILI9341_GREEN);
  }

  tft.setTextSize(1);
  for (short i = 0; i < 10; i += 1) {
    tft.setCursor(0 + (i * 32), 227);  // column,row
    tft.println(i);
  }
  tft.setCursor(300, 227);  // column,row
  tft.println("Sec");
}

void drawScreen1(){
  x = 30;
  tft.drawFastVLine(30, 0, 220, ILI9341_GREEN);
  tft.drawFastHLine(30, 220, 320, ILI9341_GREEN);
  for (short i = 0; i <= 300; i += 50) {
    tft.drawFastVLine(30 + i, 220, 5, ILI9341_GREEN);
  }

  // ----- label X axis
  tft.setTextSize(1);
  for (short i = 0; i < 300; i += 50) {
    tft.setCursor(28 + i, 227);  // column,row
    tft.println(i / 5);
  }
  tft.setCursor(300, 227);  // column,row
  tft.println("Sec");

  tft.drawChar(10, (1) * 32 + 12, 0xEB, colors[0], ILI9341_BLACK, 1);
  tft.drawChar(10, (2) * 32 + 12, 0xE9, colors[1], ILI9341_BLACK, 1);
  tft.drawChar(10, (3) * 32 + 12, 0xE0, colors[2], ILI9341_BLACK, 1);
  tft.drawChar(10, (4) * 32 + 12, 0xE1, colors[3], ILI9341_BLACK, 1);
  tft.drawChar(10, (5) * 32 + 12, 0xE2, colors[4], ILI9341_BLACK, 1);
}

void drawClock(){
  unsigned long time = micros() / 1000000;
  tft.fillRect(1,0,20,20,ILI9341_BLACK);
  tft.setCursor(1,0);
  tft.println(time);
}

void drawRawData(int value){
  drawClock();
  static short lastValue = 0;
  if(x % TIME_PER_PIXEL == 0){
    tft.drawLine(x / TIME_PER_PIXEL, lastValue + ZERO_OFFSET, (x / TIME_PER_PIXEL) + 1, -map(value, 0, 1023, 0, 127) + ZERO_OFFSET, ILI9341_WHITE);  // position trace 60 pixels below top
    lastValue = -map(value, 0, 1023, 0, 127);
    if (x > TIME_PERIODS) {
      drawScreen();
      x = 0;
    }
  }
  x++;

}

void drawBinData() {
  drawClock();
  static char lastBins[5] = { 0, 0, 0, 0, 0 };
  for(int i = 0; i < 5; i++){
    bins[i] = round(averagePower(data, binFreq[i][0]/binWidth, binFreq[i][1]/binWidth)); // 1-4 Hz
  }      

  for (short i = 0; i < 5; i++) {
    tft.drawLine(x, lastBins[i] + ((i + 1) * 32) + 16, x + 5, -bins[i] + ((i + 1) * 32) + 16, colors[i]);
    lastBins[i] = -bins[i];
  }
  x += 5;       // point to next data location
  if (x > 320) {
    drawScreen();
    x = 30;
  }
}

double averagePower(char realArray[], int start, int finish) {
  double sum;
  int coefficientCount = finish - start;
  for (start; start < finish; start++) {
    sum = (sq(realArray[start]));
  }
  return sqrt(sum / coefficientCount);
}