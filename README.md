# Arduino-EEG-Experiments Notebook

This code is the messiest I've ever written. I will clean it up.
- Features
  - Reads sensor value via analog pin
  - outputs via serial, BLE, and writes to microSD card
  - uses RTC solely for timestamping the overall file; timestamps for individual records are done in micros()
  - performs FFT