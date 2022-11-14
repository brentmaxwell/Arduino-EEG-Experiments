int[][] colors = new int[][] {
  {255, 255, 255},  // white  
  {255,   0,   0},  // red 
  {255, 128,   0},  // orange 
  {255, 255,   0},  // yellow
  {  0, 255,   0},  // green
  {  0,   0, 255},  // blue
  {255,   0, 255},  // purple 
};
int[][] binColors = new int[32][3];

int[][] bins = new int[5][0];

void calculateBinColors() {
  for(int i=0;i<32;i++){
    int colorVal = i%colors.length;
    binColors[i] = colors[colorVal];
  }
}
    
void calculateBins() {
  int[][] binRange = {
    {0,4},
    {4,8},
    {8,12},
    {12,32},
    {32,maxF}
  };
  
  for(int i=0; i<32;i++)
  {
    int midFreq = (i*binWidth) + (binWidth/2);
    for(int j=0; j<binRange.length; j++){
      if(midFreq >= binRange[j][0] && midFreq < binRange[j][1]){
        bins[j] = append(bins[j],i);
      }
    }
  }
  for(int i=0; i<bins.length; i++)
  {
    bins[i] = sort(bins[i]);
  }
}
