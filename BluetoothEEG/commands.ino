typedef volatile uint32_t REG32;
#define pREG32 (REG32 *)

#define DEVICE_ID_HIGH (*(pREG32(0x10000060)))
#define DEVICE_ID_LOW (*(pREG32(0x10000064)))
#define MAC_ADDRESS_HIGH (*(pREG32(0x100000a8)))
#define MAC_ADDRESS_LOW (*(pREG32(0x100000a4)))

void loopCommand() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      // if the incoming character is a newline, set a flag so the main loop can
      // do something about it:
      commandSource = 0;
      stringComplete = true;
    } else {
      // add it to the inputString:
      commandString += inChar;
    }
  }
  if (stringComplete) {
    if (commandSource == 0) {
      Serial.println(">> " + commandString);
      Serial.flush();
    }
    if (commandSource == 1) {
      writeBle(">> " + commandString + "\n");
    }
    if (commandString.startsWith("m")) Run(EventMarkerCommand(commandString));
    if (commandString.startsWith("list")) Run(ListFilesCommand(commandString));
    if (commandString.startsWith("start")) Run(StartCommand(commandString));
    if (commandString.startsWith("stop")) Run(StopCommand(commandString));
    if (commandString.startsWith("hwinfo")) HwInfoCommand(commandString);
    if (commandString.startsWith("date")) Run(DateCommand(commandString));
    if (commandString.startsWith("set time")) Run(SetTimeCommand(commandString));
    if (commandString.startsWith("batt")) Run(BattCommand(commandString));
    if (commandString.startsWith("reset")) ResetCommand(commandString);
    if (commandString.startsWith("led")) LedCommand(commandString);
    Serial.flush();
    commandString = "";
    stringComplete = false;
  }
}

void Run(String output) {
  switch (commandSource) {
    case OUTPUT_SERIAL:
      Serial.print(output);
      Serial.flush();
      break;
    case OUTPUT_BLE:
      writeBle(output);
      break;
  }
}

String ListFilesCommand(String commandString) {
  String arg1;
  get_token(commandString, arg1, 1, ' ');
  if (arg1 == "") arg1 = "/";
  return listFiles();
}

void ResetCommand(String commandString) {
  String arg1;
  get_token(commandString, arg1, 1, ' ');
  if (arg1 == "uf2") {
    enterUf2Dfu();
  } else {
    NVIC_SystemReset();
  }
}

String BattCommand(String commandString){
  String output = "";
  output += batt.getBatteryLevel();
  output += " (";
  output += batt.getBatteryPercent();
  output += ")\n";
  return output;
}

String DateCommand(String commandString) {
  return rtcClock.getDateTime() + "\n";
}

void LedCommand(String commandString) {
  String r;
  String g;
  String b;
  get_token(commandString, r, 1, ' ');
  get_token(commandString, g, 2, ' ');
  get_token(commandString, b, 3, ' ');
  statusLed.setColor(r.toInt(), g.toInt(), g.toInt());
}

String SetTimeCommand(String commandString) {
  String year;
  String month;
  String day;
  String hour;
  String minute;
  String second;
  get_token(commandString, year, 2, ' ');
  get_token(commandString, month, 3, ' ');
  get_token(commandString, day, 4, ' ');
  get_token(commandString, hour, 5, ' ');
  get_token(commandString, minute, 6, ' ');
  get_token(commandString, second, 7, ' ');
  rtcClock.setDateTime(year.toInt(), month.toInt(), day.toInt(), hour.toInt(), minute.toInt(), second.toInt());
  return rtcClock.getDateTime() + "\n";
}

String StartCommand(String commandString) {
  String arg1;
  get_token(commandString, arg1, 1, ' ');
  if (arg1 == "serial") {
    output_serial = true;
    return F("Serial output started\n");
  }
  if (arg1 == "ble") {
    output_ble = true;
    return F("BLE output started\n");
  }
  return "";
}

String StopCommand(String commandString) {
  String arg1;
  get_token(commandString, arg1, 1, ' ');
  if (arg1 == "serial") {
    output_serial = false;
    return F("Serial output stopped\n");
  }
  if (arg1 == "ble") {
    output_ble = false;
    return F("BLE output stopped\n");
  }
  return "";
}

String EventMarkerCommand(String commandString) {
  writeMessage(timer_count, timer, commandString);
  return commandString + "\n";
}

bool get_token(String &from, String &to, uint8_t index, char separator) {
  uint16_t start = 0, idx = 0;
  uint8_t cur = 0;
  while (idx < from.length()) {
    if (from.charAt(idx) == separator) {
      if (cur == index) {
        to = from.substring(start, idx);
        return true;
      }
      cur++;
      while ((idx < from.length() - 1) && (from.charAt(idx + 1) == separator)) idx++;
      start = idx + 1;
    }
    idx++;
  }
  if ((cur == index) && (start < from.length())) {
    to = from.substring(start, from.length());
    return true;
  }
  return false;
}

void HwInfoCommand(String commandString) {
  // String arg1;
  // get_token(commandString, arg1, 1, ' ');
  Serial.println("--HW Info--");
  Serial.printf("Serial No   : %s\r\n", getMcuUniqueID());
  Serial.printf("CPU Speed  : %d MHz\n", F_CPU / 1000000);
  // MAC Address
  uint32_t addr_high = ((MAC_ADDRESS_HIGH)&0x0000ffff) | 0x0000c000;
  uint32_t addr_low = MAC_ADDRESS_LOW;
  Serial.print("MAC Address: ");
  Serial.print((addr_high >> 8) & 0xFF, HEX);
  Serial.print(":");
  Serial.print((addr_high)&0xFF, HEX);
  Serial.print(":");
  Serial.print((addr_low >> 24) & 0xFF, HEX);
  Serial.print(":");
  Serial.print((addr_low >> 16) & 0xFF, HEX);
  Serial.print(":");
  Serial.print((addr_low >> 8) & 0xFF, HEX);
  Serial.print(":");
  Serial.print((addr_low)&0xFF, HEX);
  Serial.println("");
  Serial.print("Device ID  : ");
  Serial.print(DEVICE_ID_HIGH, HEX);
  Serial.println(DEVICE_ID_LOW, HEX);
  Serial.printf("MCU Variant: nRF%X 0x%08X\n", NRF_FICR->INFO.PART, NRF_FICR->INFO.VARIANT);
  Serial.printf("Memory     : Flash = %d KB, RAM = %d KB\n", NRF_FICR->INFO.FLASH, NRF_FICR->INFO.RAM);
  Serial.println("--Firmware Info--");
  Serial.printf("BSP Library : " ARDUINO_BSP_VERSION "\r\n");
  Serial.printf("Bootloader  : %s\r\n", getBootloaderVersion());
  Serial.printf("\n");
  Serial.flush();
}
