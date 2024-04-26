void setup() {
  Serial.begin(115200);
  Wire.begin(7, 6);  //Join I2C bus

  if (display.begin() == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1)
      ;
  }
  Serial.println("Display acknowledged.");

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  display.setBrightness(4);

  bool isFirstBoot = checkFirstBoot();
  // If it's the first boot, perform initialization
  if (isFirstBoot) {
    Serial.println("First Boot");
    // Set the initial value to 5000
    health = 5000;
    // Save the initial value to file
    saveHealth();
  } else {
    // Restore integer value from file
    restoreHealth();
  }


  pinMode(ATTACK_LED, OUTPUT);
  pinMode(DEFEND_LED, OUTPUT);

  pinMode(SCISSOR, INPUT_PULLUP);
  pinMode(ROCK, INPUT_PULLUP);
  pinMode(PAPER, INPUT_PULLUP);
  pinMode(SCAN, INPUT_PULLUP);

  if (digitalRead(ROCK) == 0 && digitalRead(PAPER) == 0 && digitalRead(SCISSORS) == 0) {
    health += 300;
    saveHealth();
  } else if (digitalRead(SCAN) == 0 && digitalRead(ROCK) == 1 && digitalRead(PAPER) == 1 && digitalRead(SCISSORS) == 1){
    nameGlitch = true; //if hold scan during boot, glitch display name while flashing
  }
  // Save the flag indicating it's not the first boot
  saveBootFlag();

  Serial.println("Starting BLE work!");
  BLEDevice::init(DEV_NAME);
  BLEDevice::setPower(ESP_PWR_LVL_N3, ESP_BLE_PWR_TYPE_DEFAULT);
  BLEDevice::setPower(ESP_PWR_LVL_N3, ESP_BLE_PWR_TYPE_ADV);
  BLEDevice::setPower(ESP_PWR_LVL_N3, ESP_BLE_PWR_TYPE_SCAN);
  BLEServer* pServer = BLEDevice::createServer();

  // Set the callback functions for connect and disconnect events
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic* pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  // Set the callback function for write events
  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("BSIDES 2024");
  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  timer1.start();
  timer2.start();

  Serial.println("Exiting setup...");
  SerialSAO.println(flag); //prints secret flag before serial initialization. Requires dumping firmware to read.

  if (boot()) { // check for cheat code
    SerialSAO.setRxBufferSize(1024);
    SerialSAO.begin(115200, SERIAL_8N1, 10, 9);
    secret_mode();
  }
}

void secret_mode() { //WOPR
  exitState = false;
  authenticated = false;
  display.clear();
  display.print("GAME");
  display.updateDisplay();
  while (!SerialSAO) {}  //game stuff here
  clear();
  println("                                                                               ");
  uint8_t flag[32] = { 0x0a, 0xe4, 0xb8, 0x1d, 0xca, 0xe8, 0x37, 0x6b, 0x54, 0x56, 0x7c, 0x5f, 0xe2, 0x44, 0xce, 0x2a, 0x39, 0x9c, 0xa1, 0xd1, 0xb8, 0x25, 0x4d, 0x5e, 0xcf, 0x07, 0xce, 0x11, 0x79, 0xc0, 0xb3, 0xb6 };
  println(flagDecrypt(flag));
  println();
  while (!insecure_login()) {}  //loops login until passes
  authenticated = true;
  println("ACCESS GRANTED");
  modem_glitch();
  delay(1000);
  clear();
  delay(1000);
  println("GREETINGS PROFESSOR FALKEN.");
  println();
  println("SHALL WE PLAY A GAME?");
  println();
  while (!exitState) {
    receive();
    println();
    receive_handle();
    empty_rx();
  }
}

void loop() {
  timer1.update();
  timer2.update();

  digitalWrite(ATTACK_LED, LOW);  // HIGH is LED On
  digitalWrite(DEFEND_LED, HIGH);

  int scanButton = digitalRead(SCAN);
  if (scanButton == LOW && !clientConnected) {
    BLEDevice::stopAdvertising();  // didnt seem to solve bug 3...maybe beecause I did startAdvertising instead of stop..... need to try with stopAdvertising -- 7PM 3/25
    handle_scan();
  }
}
