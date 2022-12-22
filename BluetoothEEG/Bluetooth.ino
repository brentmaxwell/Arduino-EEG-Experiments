#define UUID_EEG_SERVICE "42656874-6572-746E-4545-470000000000"
#define UUID_EEG_RAW_DATA_CHARACTERISTIC "42656874-6572-746E-4545-472044617461"
#include <bluefruit.h>

// BLE Service
BLEDfu bledfu;    // OTA DFU service
BLEDis bledis;    // device information
BLEUart bleuart;  // uart over ble
BLEBas blebas;    // battery

// HRM Client
BLEClientService heartRateClient(UUID16_SVC_HEART_RATE);
BLEClientCharacteristic heartRateValueClientCharacteristic(UUID16_CHR_HEART_RATE_MEASUREMENT);
BLEClientCharacteristic heartRateSensorClientCharacteristic(UUID16_CHR_BODY_SENSOR_LOCATION);

// HRM Service
BLEService hrms = BLEService(UUID16_SVC_HEART_RATE);
BLECharacteristic hrmc = BLECharacteristic(UUID16_CHR_HEART_RATE_MEASUREMENT);
BLECharacteristic bslc = BLECharacteristic(UUID16_CHR_BODY_SENSOR_LOCATION);

// EEG Data Service
BLEService eegData = BLEService(UUID_EEG_SERVICE);
BLECharacteristic eegDataCharacteristic = BLECharacteristic(UUID_EEG_RAW_DATA_CHARACTERISTIC);



void scan_callback(ble_gap_evt_adv_report_t* report);
void hrm_notify_callback(BLEClientCharacteristic* chr, uint8_t* data, uint16_t len);
void hrm_cccd_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint16_t cccd_value);
void eeg_cccd_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint16_t cccd_value);

bool setupBLE() {
  Bluefruit.autoConnLed(false);
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.begin(1, 1);
  Bluefruit.setTxPower(4);       // Check bluefruit.h for supported values
  Bluefruit.setName(BTLE_NAME);  // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  bledfu.begin();
  bledis.begin();
  bledis.setManufacturer(BTLE_MANUFACTURER);
  bledis.setModel(BTLE_MODEL);
  bleuart.begin();
  blebas.begin();
  blebas.write(100);
  setupHrmService();
  startAdv();
  setupHrmClient();
  return true;
}

bool setupHrmClient() {
  heartRateClient.begin();
  heartRateSensorClientCharacteristic.begin();
  heartRateValueClientCharacteristic.setNotifyCallback(hrm_notify_callback);
  heartRateValueClientCharacteristic.begin();
  Bluefruit.Central.setDisconnectCallback(hrm_disconnect_callback);
  Bluefruit.Central.setConnectCallback(hrm_connect_callback);
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80);  // in unit of 0.625 ms
  Bluefruit.Scanner.filterUuid(heartRateClient.uuid);
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);  // // 0 = Don't stop scanning after n seconds
  return true;
}

void setupEegService(void) {
  hrms.begin();

  // Note: You must call .begin() on the BLEService before calling .begin() on
  // any characteristic(s) within that service definition.. Calling .begin() on
  // a BLECharacteristic will cause it to be added to the last BLEService that
  // was 'begin()'ed!

  // Configure the Heart Rate Measurement characteristic
  // See: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.heart_rate_measurement.xml
  // Properties = Notify
  // Min Len    = 1
  // Max Len    = 8
  //    B0      = UINT8  - Flag (MANDATORY)
  //      b5:7  = Reserved
  //      b4    = RR-Internal (0 = Not present, 1 = Present)
  //      b3    = Energy expended status (0 = Not present, 1 = Present)
  //      b1:2  = Sensor contact status (0+1 = Not supported, 2 = Supported but contact not detected, 3 = Supported and detected)
  //      b0    = Value format (0 = UINT8, 1 = UINT16)
  //    B1      = UINT8  - 8-bit heart rate measurement value in BPM
  //    B2:3    = UINT16 - 16-bit heart rate measurement value in BPM
  //    B4:5    = UINT16 - Energy expended in joules
  //    B6:7    = UINT16 - RR Internal (1/1024 second resolution)
  hrmc.setProperties(CHR_PROPS_NOTIFY);
  hrmc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  hrmc.setFixedLen(2);
  hrmc.setCccdWriteCallback(hrm_cccd_callback);  // Optionally capture CCCD updates
  hrmc.begin();
  uint8_t hrmdata[2] = { 0b00000110, 0x40 };  // Set the characteristic to use 8-bit values, with the sensor connected and detected
  hrmc.write(hrmdata, 2);

  // Configure the Body Sensor Location characteristic
  // See: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.body_sensor_location.xml
  // Properties = Read
  // Min Len    = 1
  // Max Len    = 1
  //    B0      = UINT8 - Body Sensor Location
  //      0     = Other
  //      1     = Chest
  //      2     = Wrist
  //      3     = Finger
  //      4     = Hand
  //      5     = Ear Lobe
  //      6     = Foot
  //      7:255 = Reserved
  bslc.setProperties(CHR_PROPS_READ);
  bslc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  bslc.setFixedLen(1);
  bslc.begin();
  bslc.write8(1);  // Set the characteristic to 'Wrist' (2)
}

void setupHrmService(void) {
  eegData.begin();

  // Note: You must call .begin() on the BLEService before calling .begin() on
  // any characteristic(s) within that service definition.. Calling .begin() on
  // a BLECharacteristic will cause it to be added to the last BLEService that
  // was 'begin()'ed!

  // Configure the Heart Rate Measurement characteristic
  // See: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.heart_rate_measurement.xml
  // Properties = Notify
  // Min Len    = 1
  // Max Len    = 8
  //    B0      = UINT8  - Flag (MANDATORY)
  //      b5:7  = Reserved
  //      b4    = RR-Internal (0 = Not present, 1 = Present)
  //      b3    = Energy expended status (0 = Not present, 1 = Present)
  //      b1:2  = Sensor contact status (0+1 = Not supported, 2 = Supported but contact not detected, 3 = Supported and detected)
  //      b0    = Value format (0 = UINT8, 1 = UINT16)
  //    B1      = UINT8  - 8-bit heart rate measurement value in BPM
  //    B2:3    = UINT16 - 16-bit heart rate measurement value in BPM
  //    B4:5    = UINT16 - Energy expended in joules
  //    B6:7    = UINT16 - RR Internal (1/1024 second resolution)
  eegDataCharacteristic.setProperties(CHR_PROPS_NOTIFY);
  eegDataCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  eegDataCharacteristic.setFixedLen(2);
  eegDataCharacteristic.setCccdWriteCallback(eeg_cccd_callback);  // Optionally capture CCCD updates
  eegDataCharacteristic.begin();
  uint8_t eegdata[2] = { 0x0, 0x0 };  // Set the characteristic to use 8-bit values, with the sensor connected and detected
  eegDataCharacteristic.write(eegdata, 2);
}

void startAdv(void) {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(bleuart);
  Bluefruit.Advertising.addService(hrms);
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
  bleConnected = true;
  //dumbdisplay.playbackLayerSetupCommands("raw_eeg");
  statusLed.bluetoothStatus(true);
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  (void)conn_handle;
  (void)reason;
  Serial.println();
  Serial.print("Disconnected, reason = 0x");
  Serial.println(reason, HEX);
  statusLed.bluetoothStatus(false);
  bleConnected = false;
}

void hrm_cccd_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint16_t cccd_value) {
  // Display the raw request packet
  Serial.print("CCCD Updated: ");
  //Serial.printBuffer(request->data, request->len);
  Serial.print(cccd_value);
  Serial.println("");

  // Check the characteristic this CCCD update is associated with in case
  // this handler is used for multiple CCCD records.
  if (chr->uuid == hrmc.uuid) {
    if (chr->notifyEnabled(conn_hdl)) {
      Serial.println("Heart Rate Measurement 'Notify' enabled");
    } else {
      Serial.println("Heart Rate Measurement 'Notify' disabled");
    }
  }
}

void eeg_cccd_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint16_t cccd_value) {
  // Display the raw request packet
  Serial.print("EEG CCCD Updated: ");
  //Serial.printBuffer(request->data, request->len);
  Serial.print(cccd_value);
  Serial.println("");

  // Check the characteristic this CCCD update is associated with in case
  // this handler is used for multiple CCCD records.
  if (chr->uuid == eegDataCharacteristic.uuid) {
    if (chr->notifyEnabled(conn_hdl)) {
      Serial.println("EEG Measurement 'Notify' enabled");
    } else {
      Serial.println("EEG Measurement 'Notify' disabled");
    }
  }
}


void scan_callback(ble_gap_evt_adv_report_t* report) {
  Bluefruit.Central.connect(report);
}

void hrm_connect_callback(uint16_t conn_handle) {
  Serial.println("Connected");
  Serial.print("Discovering HRM Service ... ");
  if (!heartRateClient.discover(conn_handle)) {
    Serial.println("Found NONE");
    statusLed.bluetoothStatus(false);
    heart_rate.isValid = false;
    Bluefruit.disconnect(conn_handle);
    return;
  }
  Serial.println("Found it");
  Serial.print("Discovering Measurement characteristic ... ");
  if (!heartRateValueClientCharacteristic.discover()) {
    Serial.println("not found !!!");
    Serial.println("Measurement characteristic is mandatory but not found");
    Bluefruit.disconnect(conn_handle);
    statusLed.bluetoothStatus(false);
    heart_rate.isValid = false;
    return;
  }
  Serial.println("Found it");
  Serial.print("Discovering Body Sensor Location characteristic ... ");
  if (heartRateSensorClientCharacteristic.discover()) {
    Serial.println("Found it");
    const char* body_str[] = { "Other", "Chest", "Wrist", "Finger", "Hand", "Ear Lobe", "Foot" };
    uint8_t loc_value = heartRateSensorClientCharacteristic.read8();
    Serial.print("Body Location Sensor: ");
    Serial.println(body_str[loc_value]);
    statusLed.bluetoothStatus(true);
  } else {
    Serial.println("Found NONE");
    statusLed.bluetoothStatus(false);
    heart_rate.isValid = false;
  }
  if (heartRateValueClientCharacteristic.enableNotify()) {
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
  Serial.print(data[0]);
  Serial.print(":");
  Serial.println(data[1]);
  uint8_t hrValue;
  if (data[0] & bit(0)) {
    uint16_t value;
    memcpy(&value, data + 1, 2);
    hrValue = value;
  } else {
    hrValue = data[1];
  }
  hrmc.notify(data, sizeof(data));
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
  //blebas.write(batt.getBatteryPercent());
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
