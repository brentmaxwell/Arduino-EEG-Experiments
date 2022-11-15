final String i2cbus = "i2c-1";

import grafica.*;
import ddf.minim.analysis.*;
import ddf.minim.*;
import processing.serial.*;
import java.util.Random;
import processing.io.*;
I2C i2c;

Minim minim;
FFT fftLin;
Serial serialPort; // Serial port object

public GPlot plotTime, plotFreq, plotHist, freqAxis;

// indexes
final int DELTA = 1; // 1-4 Hz
final int THETA = 2; // 4-8 Hz
final int ALPHA = 3; // 7-13 Hz
final int BETA = 4; // 13-30 Hz
final int GAMMA = 5; // 30-55 Hz

public int Fs = 250;
int lastStepTime = 0;

int bufferSize = 1024;
float angle = 0;

float[] ticks = new float[] { 16.38, 28.67, 57.34, 114.59, 237.57 };

// Prepare the points for the first plot  
GPointsArray pointsTime = new GPointsArray(bufferSize);
GPointsArray pointsFreq = new GPointsArray(bufferSize);
GPointsArray pointsHist = new GPointsArray(5);
  
float[] rawPoints = new float[bufferSize];
float[] filt_data = new float[bufferSize];
float[] notch_filt_data = new float[bufferSize];
byte[] inBuffer = new byte[256]; // holds serial message

public void setup() {
  size(1280, 720);

  minim = new Minim(this);
  fftLin = new FFT( bufferSize, Fs );
  fftLin.window( FFT.BLACKMAN );
  
  for (int i = 0; i < bufferSize; i++) {
    pointsTime.add(i, 0);
    pointsFreq.add(i, 0);
    rawPoints[i] = 0;
  }

  // Setup for the first plot
  plotTime = new GPlot(this);
  plotTime.setPos(0, 0);
  plotTime.setDim(850, 280);
  plotTime.setXLim(100, bufferSize-128);
  plotTime.setYLim(-2000, 2000);
  plotTime.drawGridLines(GPlot.BOTH);
  plotTime.setPoints(pointsTime);
  plotTime.setLineColor(color(100, 0, 255));

  // Setup for the third plot 
  plotFreq = new GPlot(this);
  plotFreq.setPos(0, 350);
  plotFreq.setDim(850, 280);
  plotFreq.setYLim(-50, 256);
  plotFreq.setXLim(0, bufferSize/4);
  plotTime.drawGridLines(GPlot.BOTH);
  plotFreq.setHorizontalAxesTicks(ticks);
  plotFreq.setPoints(pointsFreq);
  plotFreq.setLineColor(color(100, 100, 255));
  
  // Initialize point3 data for the plotHist
  pointsHist.add(0,DELTA);
  pointsHist.add(0,THETA);
  pointsHist.add(0,ALPHA);
  pointsHist.add(0,BETA);
  pointsHist.add(0,GAMMA);
  
  // Setup for the third plot 
  plotHist = new GPlot(this);
  plotHist.setPos(920, 0);
  plotHist.setDim(250, 620);
  plotHist.setYLim(0, 6);
  plotHist.setXLim(0, 255);
  plotHist.startHistograms(GPlot.HORIZONTAL);
  //plotHist.getHistogram().setDrawLabels(true);
  //plotHist.getHistogram().setRotateLabels(true);
  plotHist.setPoints(pointsHist);
  plotHist.getHistogram().setBgColors(new color[] {
    color(75, 0, 130, 200), color(168, 20, 231, 200),
    color(100, 100, 255, 200), color(0, 255, 255, 150), color(255, 255, 00, 80)
  });
 
  freqAxis = new GPlot(this);
  freqAxis.setPos(0, 620);
  freqAxis.setDim(850, 10);
  freqAxis.setXLim(0, Fs/4);
  freqAxis.setHorizontalAxesNTicks(15);
  freqAxis.drawXAxis();

  setupI2C();
  //serialPort.bufferUntil('\n');
}

void setupI2C(){
  i2c = new I2C(i2cbus);
  //size(1024, 820);
  i2c.beginTransmission(0x48);
  // Select configuration register
  i2c.write(0x01);
  // AINP = AIN0 and AINN = AIN1, +/- 2.048V
  i2c.write(0x84);
  // Continuous conversion mode, 128 SPS
  i2c.write(0x83);
  // Stop I2C Transmission
  i2c.endTransmission();
  delay(300);
}

float readData()
{
  try{
    i2c.beginTransmission(0x48);
    i2c.write(0x00);
    byte[] input_data = i2c.read(2);
    i2c.endTransmission();
    int val = (input_data[0] & 0xff) << 8 | (input_data[1] & 0xff);
    return float(val);
  }
  catch(Exception e)
  {
    println('!');//e.printStackTrace();
    return 0;
  }
}


public void draw() {
  float val2;
  background(255);
  
  plotTime.beginDraw();
  plotTime.drawBackground();
  plotTime.drawBox();
  plotTime.drawXAxis();
  plotTime.drawYAxis();
  plotTime.drawGridLines(GPlot.BOTH);
  plotTime.drawLines();
  plotTime.endDraw();

  plotHist.beginDraw();
  plotHist.drawBackground();
  plotHist.drawBox();
  plotHist.drawHistograms();
  plotHist.endDraw();
  
  try {
    val2 = readData();
  }
  catch(Exception e) {
    val2 = 0;
  }
    
  for (int i = 1; i < bufferSize; i++) {
    rawPoints[i-1] = rawPoints[i];
  }
     
  rawPoints[bufferSize-1] = val2;

  filterIIR(notch_b2, notch_a2, rawPoints, filt_data);
 
  //null filter for test
  //filterIIR(test_b, test_a, rawPoints, filt_data);
  
  for (int i = 0; i < bufferSize; i++) { 
    pointsTime.setY(i, filt_data[i] );
  }
  fftLin.forward(filt_data);

  for (int i = 0; i < bufferSize; i++) { 
    //pointsFreq.setY(i, (fftLin.getBand(i)*0.1)-0);
    pointsFreq.setY(i, (20 * log(2*fftLin.getBand(i)/fftLin.timeSize())));
  }
    
  pointsHist.setX(0,0.0005*fftLin.calcAvg(0.1,4));
  pointsHist.setX(1,0.0005*fftLin.calcAvg(4,8.0));
  pointsHist.setX(2,0.0005*fftLin.calcAvg(8.0,13.0));
  pointsHist.setX(3,0.0005*fftLin.calcAvg(13.0,30.0));
  pointsHist.setX(4,0.0005*fftLin.calcAvg(30.0,128.0));
  
  plotTime.setPoints(pointsTime);
  plotFreq.setPoints(pointsFreq);
  plotHist.setPoints(pointsHist);

  plotFreq.beginDraw();
  plotFreq.drawBackground();
  plotFreq.drawBox();
  plotFreq.drawTitle();
  plotFreq.drawGridLines(GPlot.BOTH);
  plotFreq.drawLines();
  plotFreq.endDraw();

  freqAxis.beginDraw();
  freqAxis.drawXAxis();
  freqAxis.endDraw();
}
