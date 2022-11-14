PGraphics Canvas;                                //name of drawing area to be created
PFont myFont;                                    // name of font to be created
int X = 0;                                       // data column

void setupScreen(){
  background(0);                                 // black
  frameRate(60);                                 // 60 frames per second
  myFont = createFont("Arial Black", 18);
  Canvas = createGraphics(width, height);
}

void drawGrid()
{
  pushMatrix();

  scale(0.8);
  translate(width*0.1, height*0.10);  
  fill(0);
  stroke(255);

  // ----- border
  strokeWeight(4);
  rect(0, 0, width, height, 10, 10, 10, 10);

  // ----- grid
  strokeWeight(1);                                // thin lines
  stroke(255, 32);                                // white, 12.5% alpha

  for (int y=0; y<33; y++) {                      // horizontal lines
    line(0, height*y/33, width, height*y/33);
  }

  for (int x=1; x<61; x++) {                      // vertical lines
    line(width*x/62, 0, width*x/62, height);
  }
  
  //strokeWeight(1);
  stroke(255);
  for(int i=0;i<bins.length;i++){
    int max = bins[i][bins[i].length-1]+1;
    line(0, (height*max/33)+(height/33), width, (height*max/33)+(height/33)); //Theta waves
  }
  
  // ----- print heading
  textAlign(CENTER, BOTTOM);
  fill(255);                                    //white text
  text("Bin", -25, -20);
  text("EEG Monitor", width/2, -20);
  
  // ----- label the X-axis
  textAlign(CENTER, TOP);
  for (int x=0; x<61; x+=5) {
    text(x, width*x/60, height+10);
  }  
  text("Seconds", width/2, height+40);

  // ----- label the y-axis
  textAlign(RIGHT, CENTER);
  for (int y=0; y<32; y++) {
    fill(binColors[y][0],binColors[y][1],binColors[y][2]);
    text(((y*binWidth)) + "Hz", -20, height*(y+1)/33);
  }  
  textAlign(RIGHT, BOTTOM);
  text("Close", width, -20);

  popMatrix();
}
