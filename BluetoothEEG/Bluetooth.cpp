// #define BTLE_NAME "theBrent EEG"
// #define BTLE_MANUFACTURER "theBrent"
// #define BTLE_MODEL "EEG Headband v0.9"

// #include <bluefruit.h>

// class Bluetooth{
//   private:
//     BLEDfu _bledfu;    // OTA DFU service
//     BLEDis _bledis;    // device information
//     BLEUart _bleuart;  // uart over ble
//     BLEBas _blebas;    // battery
//     BLEClientService _heartRateClient;
//     BLEClientCharacteristic _heartRateValueClientCharacteristic;
//     BLEClientCharacteristic _heartRateSensorClientCharacteristic;
//     ble_connect_callback_t _connect_callback;
//     ble_disconnect_callback_t _disconnect_callback;
//     ble_connect_callback_t _hrm_connect_callback;
//     ble_disconnect_callback_t _hrm_disconnect_callback;
//     BLEScanner::rx_callback_t _hrm_scan_callback;
//     BLEClientCharacteristic::notify_cb_t _hrm_notify_callback;
//     bool _setupHrmClient(ble_connect_callback_t hrm_connect_callback, ble_disconnect_callback_t hrm_disconnect_callback, BLEScanner::rx_callback_t hrm_scan_callback, BLEClientCharacteristic::notify_cb_t hrm_notify_callback) {
//       this->_hrm_connect_callback = hrm_connect_callback;
//       this->_hrm_disconnect_callback = hrm_disconnect_callback;
//       this->_hrm_scan_callback = hrm_scan_callback;
//       this->_hrm_notify_callback = hrm_notify_callback;
//       this->_heartRateClient.begin();
//       this->_heartRateSensorClientCharacteristic.begin();
//       this->_heartRateValueClientCharacteristic.setNotifyCallback(this->_hrm_notify_callback);
//       this->_heartRateValueClientCharacteristic.begin();
//       Bluefruit.Central.setDisconnectCallback(this->_hrm_disconnect_callback);
//       Bluefruit.Central.setConnectCallback(this->_hrm_connect_callback);
//       Bluefruit.Scanner.setRxCallback(this->_hrm_scan_callback);
//       Bluefruit.Scanner.restartOnDisconnect(true);
//       Bluefruit.Scanner.setInterval(160, 80);  // in unit of 0.625 ms
//       Bluefruit.Scanner.filterUuid(this->_heartRateClient.uuid);
//       Bluefruit.Scanner.useActiveScan(false);
//       Bluefruit.Scanner.start(0);  // // 0 = Don't stop scanning after n seconds
//       return true;
//     }
//     void _startAdv(void) {
//       Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
//       Bluefruit.Advertising.addTxPower();
//       Bluefruit.Advertising.addService(this->_bleuart);
//       Bluefruit.Advertising.addService(this->_hrms);
//       Bluefruit.ScanResponse.addName();
//       Bluefruit.Advertising.restartOnDisconnect(true);
//       Bluefruit.Advertising.setInterval(32, 244);  // in unit of 0.625 ms
//       Bluefruit.Advertising.setFastTimeout(15);    // number of seconds in fast mode
//       Bluefruit.Advertising.start(0);              // 0 = Don't stop advertising after n seconds
//     }
//   public:
//     bool begin(
//       ble_connect_callback_t connect_callback,
//       ble_disconnect_callback_t disconnect_callback,
//       ble_connect_callback_t hrm_connect_callback,
//       ble_disconnect_callback_t hrm_disconnect_callback,
//       BLEScanner::rx_callback_t hrm_scan_callback,
//       BLEClientCharacteristic::notify_cb_t hrm_notify_callback
//     ){
//       this->_connect_callback = connect_callback;
//       this->_disconnect_callback = disconnect_callback;
      
//       this->_heartRateClient = BLEClientService(UUID16_SVC_HEART_RATE);
//       this->_heartRateValueClientCharacteristic = BLEClientCharacteristic(UUID16_CHR_HEART_RATE_MEASUREMENT);
//       this->_heartRateSensorClientCharacteristic = BLEClientCharacteristic(UUID16_CHR_BODY_SENSOR_LOCATION);
//       Bluefruit.autoConnLed(false);
//       Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
//       Bluefruit.begin(1, 1);
//       Bluefruit.setTxPower(4);       // Check bluefruit.h for supported values
//       Bluefruit.setName(BTLE_NAME);  // useful testing with multiple central connections
//       Bluefruit.Periph.setConnectCallback(this->_connect_callback);
//       Bluefruit.Periph.setDisconnectCallback(this->_disconnect_callback);
//       this->_bledfu.begin();
//       this->_bledis.begin();
//       this->_bledis.setManufacturer(BTLE_MANUFACTURER);
//       this->_bledis.setModel(BTLE_MODEL);
//       this->_bleuart.begin();
//       this->_blebas.begin();
//       this->_startAdv();
//       this->_setupHrmClient(hrm_connect_callback, hrm_disconnect_callback, hrm_scan_callback, hrm_notify_callback);
//       return true;
//     }
//     void write(String message) {  
//       this->_bleuart.print(message);
//       this->_bleuart.flush();
//     }
//     bool connected = false;
// };