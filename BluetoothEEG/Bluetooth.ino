#define BTLE_NAME "theBrent EEG"
#define BTLE_MANUFACTURER "theBrent"
#define BTLE_MODEL "EEG Headband v0.9"
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>


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
  // Setup the BLE LED to be enabled on CONNECT
  Bluefruit.autoConnLed(true);

  // Config the peripheral connection with maximum bandwidth
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin(1, 1);
  Bluefruit.setTxPower(4);  // Check bluefruit.h for supported values

  Bluefruit.setName(BTLE_NAME);  // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start Device Information Service
  bledis.setManufacturer(BTLE_MANUFACTURER);
  bledis.setModel(BTLE_MODEL);
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();

  // Start BLE Battery Service
  blebas.begin();
  blebas.write(100);

  // Set up and start advertising
  startAdv();
  return true;
}

bool setupHrm() {
  hrms.begin();

  // Initialize client characteristics of HRM.
  // Note: Client Char will be added to the last service that is begin()ed.
  bslc.begin();

  // set up callback for receiving measurement
  hrmc.setNotifyCallback(hrm_notify_callback);
  hrmc.begin();

  // Callbacks for Central
  Bluefruit.Central.setDisconnectCallback(hrm_disconnect_callback);
  Bluefruit.Central.setConnectCallback(hrm_connect_callback);

  /* Start Central Scanning
   * - Enable auto scan if disconnected
   * - Interval = 100 ms, window = 80 ms
   * - Don't use active scan
   * - Filter only accept HRM service
   * - Start(timeout) with timeout = 0 will scan forever (until connected)
   */
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80);  // in unit of 0.625 ms
  Bluefruit.Scanner.filterUuid(hrms.uuid);
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);  // // 0 = Don't stop scanning after n seconds
  return true;
}

void startAdv(void) {
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);  // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(15);    // number of seconds in fast mode
  Bluefruit.Advertising.start(0);              // 0 = Don't stop advertising after n seconds
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle) {
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };

  connection->getPeerName(central_name, sizeof(central_name));

  if (central_name == "Navi") {
    AndroidBleOutput = true;
  } else {
    AndroidBleOutput = false;
  }

  Serial.print("Connected to ");
  Serial.println(central_name);
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  (void)conn_handle;
  (void)reason;
  Serial.println();
  Serial.print("Disconnected, reason = 0x");
  Serial.println(reason, HEX);
}

void scan_callback(ble_gap_evt_adv_report_t* report) {
  // Since we configure the scanner with filterUuid()
  // Scan callback only invoked for device with bleuart service advertised
  // Connect to the device with bleuart service in advertising packet
  Bluefruit.Central.connect(report);
}

void hrm_connect_callback(uint16_t conn_handle) {
  Serial.println("Connected");
  Serial.print("Discovering HRM Service ... ");

  // If HRM is not found, disconnect and return
  if (!hrms.discover(conn_handle)) {
    Serial.println("Found NONE");
    setBlue(0);
    hr_valid = false;
    Bluefruit.disconnect(conn_handle);
    return;
  }

  // Once HRM service is found, we continue to discover its characteristic
  Serial.println("Found it");
  Serial.print("Discovering Measurement characteristic ... ");

  if (!hrmc.discover()) {
    // Measurement chr is mandatory, if it is not found (valid), then disconnect
    Serial.println("not found !!!");
    Serial.println("Measurement characteristic is mandatory but not found");
    Bluefruit.disconnect(conn_handle);
    setBlue(0);
    hr_valid = false;
    return;
  }
  Serial.println("Found it");

  // Measurement is found, continue to look for option Body Sensor Location
  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.body_sensor_location.xml
  // Body Sensor Location is optional, print out the location in text if present
  Serial.print("Discovering Body Sensor Location characteristic ... ");
  if (bslc.discover()) {
    Serial.println("Found it");

    // Body sensor location value is 8 bit
    const char* body_str[] = { "Other", "Chest", "Wrist", "Finger", "Hand", "Ear Lobe", "Foot" };

    // Read 8-bit BSLC value from peripheral
    uint8_t loc_value = bslc.read8();
    sensor_location = body_str[loc_value];
    Serial.print("Body Location Sensor: ");
    Serial.println(body_str[loc_value]);
    setBlue(255);
  } else {
    Serial.println("Found NONE");
    setBlue(0);
    hr_valid = false;
  }

  // Reaching here means we are ready to go, let's enable notification on measurement chr
  if (hrmc.enableNotify()) {
    Serial.println("Ready to receive HRM Measurement value");
    hr_valid = true;
  } else {
    Serial.println("Couldn't enable notify for HRM Measurement. Increase DEBUG LEVEL for troubleshooting");
    hr_valid = false;
  }
}

void hrm_disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  (void)conn_handle;
  (void)reason;
  Serial.print("Disconnected, reason = 0x");
  Serial.println(reason, HEX);
  hr_valid = false;
  setBlue(0);
}

void hrm_notify_callback(BLEClientCharacteristic* chr, uint8_t* data, uint16_t len) {
  int hrValue;
  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.heart_rate_measurement.xml
  // Measurement contains of control byte0 and measurement (8 or 16 bit) + optional field
  // if byte0's bit0 is 0 --> measurement is 8 bit, otherwise 16 bit.
  if (data[0] & bit(0)) {
    uint16_t value;
    memcpy(&value, data + 1, 2);
    hrValue = value;
  } else {
    hrValue = data[1];
  }
  heart_rate_value = hrValue;
  if (hrValue == 0) {
    setBlue(0);
    hr_valid = false;
  } else {
    hr_valid = true;
    setBlue(255);
    //double output[1] = { hrValue };
    //writeData("H",timer,output,1, HRM_OUTPUT);
  }
}

// void loopBle() {
//   if (bleuart.available() > 0) {
//     String testString = bleuart.readString();
//     writeMessage(timer, testString);
//   }
//   blebas.write(battery_level_percent);
// }

void writeBle(String message) {
  bleuart.print(message);
  bleuart.flush();
}