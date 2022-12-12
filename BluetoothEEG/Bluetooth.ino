#include <bluefruit.h>

// BLE Service
BLEDfu bledfu;    // OTA DFU service
BLEDis bledis;    // device information
BLEUart bleuart;  // uart over ble
BLEBas blebas;    // battery

// HRM
BLEClientService hrms(UUID16_SVC_HEART_RATE);
BLEClientCharacteristic hrmc(UUID16_CHR_HEART_RATE_MEASUREMENT);
BLEClientCharacteristic bslc(UUID16_CHR_BODY_SENSOR_LOCATION);

void scan_callback(ble_gap_evt_adv_report_t* report);
void hrm_notify_callback(BLEClientCharacteristic* chr, uint8_t* data, uint16_t len);

bool setupBLE() {
  Bluefruit.autoConnLed(false);
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.begin(1, 1);
  Bluefruit.setTxPower(4);       // Check bluefruit.h for supported values
  Bluefruit.setName(BTLE_NAME);  // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  bledfu.begin();
  bledis.setManufacturer(BTLE_MANUFACTURER);
  bledis.setModel(BTLE_MODEL);
  bledis.begin();
  bleuart.begin();
  blebas.begin();
  blebas.write(100);
  startAdv();
  return true;
}

bool setupHrm() {
  hrms.begin();
  bslc.begin();
  hrmc.setNotifyCallback(hrm_notify_callback);
  hrmc.begin();
  Bluefruit.Central.setDisconnectCallback(hrm_disconnect_callback);
  Bluefruit.Central.setConnectCallback(hrm_connect_callback);
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80);  // in unit of 0.625 ms
  Bluefruit.Scanner.filterUuid(hrms.uuid);
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);  // // 0 = Don't stop scanning after n seconds
  return true;
}

void startAdv(void) {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(bleuart);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);  // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(15);    // number of seconds in fast mode
  Bluefruit.Advertising.start(0);              // 0 = Don't stop advertising after n seconds
}

void connect_callback(uint16_t conn_handle) {
  BLEConnection* connection = Bluefruit.Connection(conn_handle);
  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));
  Serial.print("Connected to ");
  Serial.println(central_name);
  statusLed.bluetoothStatus(true);
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  (void)conn_handle;
  (void)reason;
  Serial.println();
  Serial.print("Disconnected, reason = 0x");
  Serial.println(reason, HEX);
  statusLed.bluetoothStatus(false);
}

void scan_callback(ble_gap_evt_adv_report_t* report) {
  Bluefruit.Central.connect(report);
}

void hrm_connect_callback(uint16_t conn_handle) {
  Serial.println("Connected");
  Serial.print("Discovering HRM Service ... ");
  if (!hrms.discover(conn_handle)) {
    Serial.println("Found NONE");
    statusLed.bluetoothStatus(false);
    heart_rate.isValid = false;
    Bluefruit.disconnect(conn_handle);
    return;
  }
  Serial.println("Found it");
  Serial.print("Discovering Measurement characteristic ... ");
  if (!hrmc.discover()) {
    Serial.println("not found !!!");
    Serial.println("Measurement characteristic is mandatory but not found");
    Bluefruit.disconnect(conn_handle);
    statusLed.bluetoothStatus(false);
    heart_rate.isValid = false;
    return;
  }
  Serial.println("Found it");
  Serial.print("Discovering Body Sensor Location characteristic ... ");
  if (bslc.discover()) {
    Serial.println("Found it");
    const char* body_str[] = { "Other", "Chest", "Wrist", "Finger", "Hand", "Ear Lobe", "Foot" };
    uint8_t loc_value = bslc.read8();
    Serial.print("Body Location Sensor: ");
    Serial.println(body_str[loc_value]);
    statusLed.bluetoothStatus(true);
  } else {
    Serial.println("Found NONE");
    statusLed.bluetoothStatus(false);
    heart_rate.isValid = false;
  }
  if (hrmc.enableNotify()) {
    Serial.println("Ready to receive HRM Measurement value");
    heart_rate.isValid = true;
  } else {
    Serial.println("Couldn't enable notify for HRM Measurement. Increase DEBUG LEVEL for troubleshooting");
    heart_rate.isValid = false;
  }
}

void hrm_disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  (void)conn_handle;
  (void)reason;
  Serial.print("Disconnected, reason = 0x");
  Serial.println(reason, HEX);
  heart_rate.isValid = false;
  statusLed.bluetoothStatus(false);
}

void hrm_notify_callback(BLEClientCharacteristic* chr, uint8_t* data, uint16_t len) {
  int hrValue;
  if (data[0] & bit(0)) {
    uint16_t value;
    memcpy(&value, data + 1, 2);
    hrValue = value;
  } else {
    hrValue = data[1];
  }
  heart_rate.value = hrValue;
  if (hrValue == 0) {
    statusLed.bluetoothStatus(false);
    heart_rate.isValid = false;
  } else {
    heart_rate.isValid = true;
    statusLed.bluetoothStatus(true);
  }
}

void loopBle() {
  blebas.write(batt.getBatteryPercent());
  while (bleuart.available()) {
    char inChar = (char)bleuart.read();
    if (inChar == '\n') {
      commandSource = 1;
      stringComplete = true;
    } else {
      commandString += inChar;
    }
  }
}

void writeBle(String message) {
  bleuart.print(message);
  bleuart.flush();
}