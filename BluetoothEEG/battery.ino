#define VBAT_MV_PER_LSB (0.18310546875)  // 3.0V ADC range and 14-bit ADC resolution = 3000mV/4096
#define VBAT_DIVIDER (0.5F)              // 2M + 0.806M voltage divider on VBAT = (2M / (0.806M + 2M))
#define VBAT_DIVIDER_COMP (2.0F)         // Compensation factor for the VBAT divider
#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)


double readVBAT() {
  float raw;
  analogReference(AR_INTERNAL_3_0);

  // Get the raw 12-bit, 0..3000mV ADC value
  raw = analogRead(BATT_PIN);

  analogReference(AR_DEFAULT);
  // Convert the raw value to compensated mv, taking the resistor-
  // divider into account (providing the actual LIPO voltage)
  // ADC range is 0..3000mV and resolution is 12-bit (0..4095)
  return raw * REAL_VBAT_MV_PER_LSB;
}

uint8_t mvToPercent(double mvolts) {
  if (mvolts < 3300)
    return 0;

  if (mvolts < 3600) {
    mvolts -= 3300;
    return mvolts / 30;
  }

  mvolts -= 3600;
  return 100;  // thats mvolts /6.66666666
}

void getBatteryLevel() {
  // Get a raw ADC reading
  battery_level = readVBAT();

  // Convert from raw mv to percentage (based on LIPO chemistry)
  battery_level_percent = mvToPercent(battery_level);
}
