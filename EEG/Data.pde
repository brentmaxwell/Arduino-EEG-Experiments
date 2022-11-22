import processing.serial.*;
Serial serialPort;

String[] Values = new String[32];
float[] rawFftData = new float[32];                        // current data                                    
float[] LastData = new float[32];                    // previous data

String rawInputString;     // used for incoming data
int rawInputValue;

void setupSerial(){
  serialPort = new Serial(this, SerialPort, BaudRate);
  serialPort.bufferUntil('\n');
}

void serialEvent(Serial serialPort)
{
  try{
    rawInputString = serialPort.readStringUntil('\n');
    switch(rawInputString.charAt(0)){
      case 'T':
        rawFftData = float(split(rawInputString, ','));
        break;
      case 'R':
        rawInputValue = int(rawInputString);
        break;
    }
    serialPort.clear();
  }
  catch(Exception e)
  {
    println('!');
  }
}
