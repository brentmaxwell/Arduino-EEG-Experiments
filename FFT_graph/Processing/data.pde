import processing.serial.*;
Serial myPort;

String [] Values = new String[32];
float[] Data = new float[32];                        // current data                                    
float[] LastData = new float[32];                    // previous data

String input_string;     // used for incoming data

void setupSerial(){
  myPort = new Serial(this, port, Baud_rate);
  myPort.bufferUntil('\n');
}

void serialEvent(Serial myPort)
{
  try{
    input_string = myPort.readStringUntil('\n');
    if (input_string != null)
    {
      input_string = trim(input_string);
      outputData(input_string);
      parseInput(input_string);
    }  
    X++;
    if (X>60) {
      X=1;
      Canvas.clear();
    }
    myPort.clear();
  }
  catch(Exception e)
  {
    println('!');//e.printStackTrace();
  }
}

void parseInput(String data) {
  input_string = split(data,':')[1];
  
  Values = split(input_string, ',');
  Data = float(Values);
  flag = true;
}
