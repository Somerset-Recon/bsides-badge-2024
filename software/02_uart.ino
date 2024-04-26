void receive_handle();

void printFlag(String flag) {
  bool firstTime = true;
  unsigned long time;
  while (sizeof(flag) % 4 != 0) {
    flag += " ";
  }
  display.setBrightness(15);
  for (int i = 0; i < sizeof(flag) - 1; i++) {
    display.clear();
    display.print(flag.substring(i));
    display.updateDisplay();
    if (firstTime) {
      time = millis();
      while (millis() - time < 1000 && digitalRead(SCAN) == 1) {}
      firstTime = false;
    } else {
      time = millis();
      while (millis() - time < 500 && digitalRead(SCAN) == 1) {}
    }
  }
  for (int i = 0; i < 4; i++) {
    display.shiftLeft();
    display.updateDisplay();
    time = millis();
    while (millis() - time < 500 && digitalRead(SCAN) == 1) {} //skip flag being printed when scan is held
  }
  display.setBrightness(4);
}

bool checkForPress(int pin, unsigned long time) {
  unsigned long start = millis();
  while ((millis() - start < time) && (digitalRead(pin) == 1)) {
    if ((millis() - start) % 200 == 0) {
    }
  }
  start = millis();
  while (digitalRead(pin) == 0 && millis() - start < 700) {}  //gets time pressed reading
  unsigned long end = millis();
  delay(50);  //debounce
  if (end - start > 52 && end - start < 700) {
    return true;
  }
  return false;
}

bool boot() {
  String booter[4] = { "-", "--", "---", "----" };
  int timePerButton = 750;
  bool buttonPress = true;
  byte sequencePos = 0;
  static byte buttonSequence[] = { UP, UP, DOWN, DOWN, LEFT, RIGHT, LEFT, RIGHT }; //konami code
  for (int i = 0; i < 4; i++) {
    display.clear();
    display.print(booter[i]);
    display.updateDisplay();
    buttonPress = true;
    sequencePos = 0;
    while (buttonPress == true && sequencePos < sizeof(buttonSequence)) {
      buttonPress = checkForPress(buttonSequence[sequencePos], timePerButton);
      sequencePos++;
    }
    if (buttonPress == true) {  //all button sequences have been pressed correctly
      printFlag("FLAG{KONAM1_COD3R}");
      return true;
    }
  }
  return false;
}

void print(String text, int delaymin = 20, int delaymax = 50) { //serial print character by character
  for (int i = 0; i < text.length(); i++) {
    SerialSAO.print(text[i]);
    delay(random(delaymin, delaymax + 1));
  }
}
void println(String text = "", int delaymin = 20, int delaymax = 50) { //same as above, with newline
  for (int i = 0; i < text.length(); i++) {
    SerialSAO.print(text[i]);
    delay(random(delaymin, delaymax + 1));
  }
  delay(100);
  SerialSAO.println();
}
void clear() {
  SerialSAO.write(27);     // ESC command
  SerialSAO.print("[2J");  // clear screen command
  SerialSAO.print("\033[0G");
  SerialSAO.print("\033[0H");
  SerialSAO.print("\033[0m");  //reset cursor changes
}

void empty_rx() {
  while (SerialSAO.available() > 0) {
    SerialSAO.read();
  }
}

void receive() { //stores input into buffer
  memset(receivedChars, 0, numChars);
  static byte ndx = 0;
  char endMarker = '\r';
  char rc;
  bool overflow = false;  //if someone tries to buffer overflow
  while (!SerialSAO.available()) {};
  while (rc != endMarker) {
    if (SerialSAO.available() > 0) {
      rc = SerialSAO.read();
      SerialSAO.print(rc);
      if (rc == 127 || rc == 8) { //backspace/del is interpreted correctly
        SerialSAO.write(8); //go back a character
        SerialSAO.print(" "); //overwrite character with space
        SerialSAO.write(8); //go back again
        ndx--;
      } else if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
          overflow = true;
        }
      } else {
        receivedChars[ndx] = '\0';  // terminate the string
        ndx = 0;
        newData = true;
      }
    }
  }
  SerialSAO.println();
}

bool insecure_login() {
  char password[32];
  print("LOGON: ");
  receive();
  strcpy(password, receivedChars);  //overflows two bytes of auth
  empty_rx();
  if (receivedChars[16] != '\0') {
    println("PROVIDE 16 CHARACTERS MAX");
  }
  if (String(password).equalsIgnoreCase("joshua")) { //Joshua, joshua, jOsHuA, etc
    authenticated = true;
  } else {
    receive_handle();
  }
  return authenticated;
}