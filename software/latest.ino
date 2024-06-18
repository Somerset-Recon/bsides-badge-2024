#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Wire.h>
#include <SparkFun_Alphanumeric_Display.h>

// IMPORTANT NOTE TICKER 4.x does not work
#include <Ticker.h>  // Manually replaced lib in esp32 lib folder --> ~/.arduino15/packages/esp32/hardware/esp32/2.0.14/libraries/Ticker
#include <SPIFFS.h>

// Set the 4 character device name that will dispaly
const char* DEV_NAME = "TEST";

HT16K33 display;
const char* healthFilePath = "/health.txt";
const char* bootFlagPath = "/firstboot.flag";
String srv_result;
const int SCISSOR = 2;
const int ROCK = 3;
const int PAPER = 4;
const int SCAN = 5;
const int ATTACK_LED = 8;
const int DEFEND_LED = 1;
int health = 5000;  // Initialize health to 5000
bool defender = true;
bool attacker = false;
const int CLK = 7;  //PIN D5
const int DIO = 6;  //PIN D4
bool clientConnected = false;
bool nameGlitch = false;  //set to true if SCAN held during boot
bool PRINT_RESULT = false;
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static boolean displayDevName = true;  // Flag to indicate whether to display DEV_NAME or health
static boolean ATTACKING = false;
const unsigned long displayInterval = 5000;

static BLEUUID SERVICE_UUID("464c4147-7b42-4c55-335f-55553144347d");  //FLAG{BLU3_UU1D3}
static BLEUUID CHARACTERISTIC_UUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

static BLEServer* pServer;
static BLEAdvertising* pAdvertising;

static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;


//======================================= SERVER FUNCTIONS ========================================

// Get value from client and reset pCharacteristic

void onWrite(BLECharacteristic* pCharacteristic) {
  std::string clientValue = pCharacteristic->getValue();
  pCharacteristic->setValue("");
  process_client_data(pCharacteristic, clientValue);
}


class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Client connected");
    clientConnected = true;
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Client disconnected");
    clientConnected = false;
    BLEDevice::startAdvertising();
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    ::onWrite(pCharacteristic);
  }
};
// ============================================= END SERVER FUNCTIONS =========================================

// ============================================= CLIENT FUNCTIONS =============================================

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    BLEDevice::stopAdvertising();
    ATTACKING = true;
  }

  void onDisconnect(BLEClient* pclient) {
    BLEDevice::startAdvertising();
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient* pClient = BLEDevice::createClient();
  Serial.println("Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  pClient->connect(myDevice);  // For some reason the server callback for onConnect server seems to get called here .....
  Serial.println("Connected to server");
  pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(SERVICE_UUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println("Found service");


  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(CHARACTERISTIC_UUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println("Found characteristic");

  // Read the value of the characteristic.
  /*
  if (pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }
  */

  connected = true;
  return true;
}

// Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {

  // Called for each advertising BLE server.
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // Check device serviceUUID
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(SERVICE_UUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
  }
};

// ============================================= ENDCLIENT FUNCTIONS =============================================

Ticker timer1(toggleDisplay, 2000);
Ticker timer2(saveHealth, 3000);

// Client / Attacker Handler
void handle_scan() {
  digitalWrite(ATTACK_LED, HIGH);  // HIGH is LED On
  bool responseReceived = false;

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect) {
    if (connectToServer()) {
      Serial.println("Connected to the BLE Server.");
    } else {
      Serial.println("Failed to connect to the server");
    }
    doConnect = false;
  }
  if (connected && ATTACKING) {
    String newValue = "timeout";
    std::string value;
    const unsigned long waitTime = 15000;
    unsigned long startTime = millis();
    while (millis() - startTime < waitTime) {
      if (digitalRead(ROCK) == LOW) {
        newValue = "rock";
        break;  // Exit the loop if the button is pressed
      }
      if (digitalRead(PAPER) == LOW) {
        newValue = "paper";
        break;  // Exit the loop if the button is pressed
      }
      if (digitalRead(SCISSOR) == LOW) {
        newValue = "scissor";
        break;  // Exit the loop if the button is pressed
      }
    }
    digitalWrite(DEFEND_LED, LOW);
    Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());

    startTime = millis();  // Reset the timer
    while (millis() - startTime < waitTime && !responseReceived) {
      std::string value = pRemoteCharacteristic->readValue();
      if (!value.empty()) {
        Serial.print("Response received from server: ");
        Serial.println(value.c_str());
        // Process the response here
        responseReceived = true;
        if (value == "WON") {
          disconnectFromServer();
          display.print("LOST");
          health -= 50;
        } else if (value == "LOST") {
          disconnectFromServer();
          display.print("WON");
          health += 100;
        } else if (value == "TIE") {
          disconnectFromServer();
          display.print("TIE");
        } else {
          disconnectFromServer();
          display.print("ERR");
        }
        break;
      }
      delay(100);  // Delay for stability
    }
    disconnectFromServer();

  } else if (connected && !ATTACKING) {
    Serial.println("bail out");
    display.print("ERR");
  } else {
    Serial.println("No targets found");
    display.print("NODE");
    BLEDevice::startAdvertising();
  }
  ATTACKING = false;
  disconnectFromServer();
  delay(5000);
}

// Function to disconnect from the BLE server
void disconnectFromServer() {
  if (connected) {
    BLEClient* pClient = pRemoteCharacteristic->getRemoteService()->getClient();
    if (pClient != nullptr) {
      pClient->disconnect();
      connected = false;
      Serial.println("Disconnected from the BLE Server.");
    }
  }
}

void wopr_display_glitch(int);

void glitch_name() {  // Cool display when SCAN is held at boot
  for (int digit = 0; digit < 4; digit++) {
    display.setBrightness(4);
    long time = 900;
    unsigned long start = millis();
    while (millis() - start < time) {
      display.clear();
      wopr_display_glitch(digit);
      for (int i = 0; i < digit; i++) {
        display.printChar(DEV_NAME[i], i);
      }
      display.updateDisplay();
      delay(30);
    }
    display.clear();
    for (int i = 0; i < 4; i++) {
      display.printChar(DEV_NAME[i], i);
    }
    display.updateDisplay();
  }
  display.setBrightness(4);
  display.updateDisplay();
  delay(1500);
}


void toggleDisplay() {

  if (PRINT_RESULT) {
    display.print(srv_result);
    PRINT_RESULT = false;
    delay(5000);
  }

  // Check if it's time to display DEV_NAME or health
  if (clientConnected) {
    health--;
    display.print(health);
  } else {
    if (displayDevName) {
      if (nameGlitch) {
        glitch_name();
      } else {
        display.print(DEV_NAME);
      }
    } else {
      if (health > 9999) {
        display.setBrightness(16);
        String flag = "MASTER OF BSDZ";
        long time;
        bool firstTime = false;
        for (int i = 0; i < sizeof(flag) - 1; i++) {
          display.clear();
          display.print(flag.substring(i));
          display.updateDisplay();
          if (firstTime) {
            time = millis();
            while (millis() - time < 1000) {}
            firstTime = false;
          } else {
            time = millis();
            while (millis() - time < 500) {}
          }
        }
        display.setBrightness(4);
      } else {
        display.print(health);
      }
    }
    // Toggle the flag for the next iteration
    displayDevName = !displayDevName;
  }
}

void restoreHealth() {
  // Open file for reading
  File file = SPIFFS.open(healthFilePath, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  // Read the first line of the file and convert to integer
  String data = file.readStringUntil('\n');
  health = data.toInt();
  file.close();
}

void saveHealth() {
  // Open file for writing
  File file = SPIFFS.open(healthFilePath, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  // Convert integer to string and write to file
  file.println(health);
  file.close();
}

bool checkFirstBoot() {
  // Check if the first boot flag file exists
  return !SPIFFS.exists(bootFlagPath);
}


void saveBootFlag() {
  // Create the first boot flag file
  File file = SPIFFS.open(bootFlagPath, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to create first boot flag file");
    return;
  }
  file.println("First boot flag");
  file.close();
}


// SERVER / Defender Processing
void process_client_data(BLECharacteristic* pCharacteristic, const std::string& clientValue) {
  String newValue = "timeout";
  digitalWrite(ATTACK_LED, HIGH);
  const unsigned long waitTime = 15000;
  unsigned long startTime = millis();
  while ((millis() - startTime < waitTime) && clientValue != "timeout") {
    if (digitalRead(ROCK) == LOW) {
      newValue = "rock";
      digitalWrite(ATTACK_LED, LOW);
      break;
    }
    if (digitalRead(PAPER) == LOW) {
      newValue = "paper";
      digitalWrite(ATTACK_LED, LOW);
      break;
    }
    if (digitalRead(SCISSOR) == LOW) {
      newValue = "scissor";
      digitalWrite(ATTACK_LED, LOW);
      break;
    }
  }

  // Determine the result of the game
  String result;
  if (newValue == "timeout") {
    srv_result = "WON";
    health += 150;
  } else {
    if (clientValue == "rock") {
      if (newValue == "rock") {
        srv_result = "TIE";
        health += 10;
      } else if (newValue == "paper") {
        srv_result = "WON";
        health += 150;
      } else {
        srv_result = "LOST";
        health -= 25;
      }
    } else if (clientValue == "paper") {
      if (newValue == "rock") {
        srv_result = "LOST";
        health -= 25;
      } else if (newValue == "paper") {
        srv_result = "TIE";
        health += 10;
      } else {
        srv_result = "WON";
        health += 150;
      }
    } else if (clientValue == "scissor") {
      if (newValue == "rock") {
        srv_result = "WON";
        health += 150;
      } else if (newValue == "paper") {
        srv_result = "LOST";
        health -= 25;
      } else {
        srv_result = "TIE";
        health += 10;
      }
    } else if (clientValue == "timeout") {
      srv_result = "WON";
      health += 150;

    } else {
      srv_result = "invalid";  // Handle invalid input from the client
    }
  }
  pCharacteristic->setValue(srv_result.c_str());
  Serial.print("Result: ");
  Serial.println(srv_result);
  if (srv_result == "WON") {
    PRINT_RESULT = true;
  } else if (srv_result == "LOST") {
    PRINT_RESULT = true;
  } else if (srv_result == "TIE") {
    PRINT_RESULT = true;
  } else {
    srv_result = "ERR";
    PRINT_RESULT = true;
  }
}
