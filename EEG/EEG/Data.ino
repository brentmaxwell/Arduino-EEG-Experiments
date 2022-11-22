int getData() {
  return analogRead(INPUT_PIN);
}

void processFFT() {
  static const short m = (short(log10(SAMPLES) / log10(2)));
  static short dc_offset;
  static short dc_counter = 5;
  static bool data_valid = false;

  fix_fft(data, im, m, 0);
  // calculate amplitudes, ignoring image data beyond n/2 (Nyquist)
  for (short i = 0; i < SAMPLES / 2; i++) {
    data[i] = (sqrt(data[i] * data[i] + im[i] * im[i]));
  }

  // capture dc offset
  if (!data_valid && (dc_counter > 0)) {
    dc_counter--;
    if (dc_counter < 1){
      data_valid = true;
    }
    dc_offset = data[0];
  }
}