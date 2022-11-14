PrintWriter outputFile;
String filename = year() + "-" + month() + "-" + day() + "-" + hour() + "-" + minute() + "-" +  second() + ".txt";

void setupOutput(){
  outputFile = createWriter(filename);

}

void outputData(String data){
  String outputdata = millis()+":"+X+":"+data;
  println(outputdata);
  outputFile.println(outputdata);
}

void saveFile() {
  outputFile.flush();  // Writes the remaining data to the file
  outputFile.close();  // Finishes the file
}
