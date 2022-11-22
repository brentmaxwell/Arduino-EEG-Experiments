
void setup() {
  setupSerial();
}

void draw() {
}

void mouseClicked() {
  int x_min = 0;
  int x_max = 20;
  int y_min = 0;
  int y_max = 20;
  if (mouseX >= x_min && mouseX <= x_max && 
      mouseY >= y_min && mouseY <= y_max) {
    saveFile();
    exit();
  }
}
