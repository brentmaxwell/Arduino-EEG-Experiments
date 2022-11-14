boolean flag;

void setup() {
  size(1024, 820);                               // define window size, 3D
  setupScreen();
  calculateBinColors();
  calculateBins();  
  setupOutput();
  setupSerial();
}

void mouseClicked() {
  int x_min = 855;
  int x_max = 899;
  int y_min = 31;
  int y_max = 45;
  if (mouseX >= x_min && mouseX <= x_max && 
      mouseY >= y_min && mouseY <= y_max) {
    saveFile();
    exit();
  }
}
  
void draw()
{
  // ----- refresh the screen
  background(0);                      // black background
  textFont(myFont, 18);               // specify font to be used
  drawGrid();                         // draw grid
  if (flag==true) {
    flag=false;                       // only plot Data[] once otherwise it creates a tail
    plotData();                       // draw the data
  }
  image(Canvas, 0, 0);
}

void plotData()
{
  Canvas.beginDraw();
  Canvas.scale(0.8);
  Canvas.stroke(0, 255, 0);
  Canvas.strokeWeight(1);
  Canvas.translate(width*0.1, height*0.1);  
  //printArray(Data);                           // debug

  // ----- plot the EEG data
  for (int y=0; y<32; y++) {
    Canvas.stroke(binColors[y][0],binColors[y][1],binColors[y][2]);
    //if (y==0) Data[y] -= 80;                  // remove DC offset
    Canvas.line(width*(X-1)/60, (height*(y+1)/33) - height/33*LastData[y]/255*20, width*X/60, (height*(y+1)/33) - height/33*Data[y]/255*20);
  }
  
  // ----- housekeeping
  LastData = Data;
  Canvas.endDraw();
}
