int gameCount = 16;

void (*gameFuncList[16])() = { chesscheck, BJ, chesscheck, hearts, chesscheck, chesscheck, chess, chesscheck, chesscheck, GE, chesscheck, ATGA, chesscheck, chesscheck, GTW, tictactoe };

String gameList[16] = {
  "FALKEN'S MAZE",
  "BLACK JACK",
  "GIN RUMMY",
  "HEARTS",
  "BRIDGE",
  "CHECKERS",
  "CHESS",
  "POKER",
  "FIGHTER COMBAT",
  "GUERRILLA ENGAGEMENT",
  "DESERT WARFARE",
  "AIR-TO-GROUND ACTIONS",
  "THEATERWIDE TACTICAL WARFARE",
  "THEATERWIDE BIOTOXIC AND CHEMICAL WARFARE",
  "GLOBAL THERMONUCLEAR WAR",
  "TIC-TAC-TOE",
};
String weird_code[] = {
  "145          11456          11889          11893                                \n",
  "PRT CON. 3.4.5. SECTRAN 9.4.3.          PORT STAT: SB-345                      \n",
  "                                                                                \n",
  "\033[2J",
  "(311) 655-7385                                                                 \n",
  "                                                                                \n",
  "                                                                                \n",
  "\033[2J",
  "(311) 767-8739                                                                 \n",
  "(311) 936-2364                                                                 \n",
  "\033[2J",
  "\033[7mPRT. STAT.                   CRT. DEF.                                    \033[0m\n",
  "================================================                            \n",
  "\033[7mFSKJJSJ: SUSJKJ: SUFJSL:          DKSJL: SKFJJ: SDKFJLJ                   \033[0m\n",
  "\033[7mSYSPROC FUNCT READY          ALT NET READY                                \033[0m\n",
  "\033[7mCPU AUTH RY-345-AX3     SYSCOMP STATUS: ALL PORTS ACTIVE                  \033[0m\n",
  "22/34534.90/3289               CVB-3904-39490                              \n",
  "(311) 936-2384                                                                 \n",
  "(311) 936-3582                                                                 \n",
  "\033[2J",
  "22/34534.3209                  CVB-3904-39490                              \n",
  "12934-AD-43KJ: CENTR PAK                                                      \n",
  "(311) 767-1083                                                                 \n",
  "\033[2J",
  "\n",
  "\n",
  "\n",
  "\n",
  "\n",
  "\n",
  "\n",
  "\n",
  "\n",
  "\n",
  "\n",
  "\n",
  "\033[7mFLD CRS: 33.34.543     HPBS: 34/56/67/83     STATUS FLT  034/304          \033[0m\n",
  "\033[7m1105-45-F6-B456          NOPR STATUS: TRAK OFF     PRON ACTIVE            \033[0m\n",
  "(45:45:45  WER: 45/29/01 XCOMP: 43239582 YCOMP: 3492930D ZCOMP: 343906834        \n",
  "                                          SRON: 65=65/74/84/65/89            \n",
  "\033[2J\033[H                                                                 \n",
  "-           PRT. STAT.                        CRY. DEF.                      \n",
  "(311) 936-1582==============================================                \n",
  "                  3453                3594                                   \n",
  "\n",
  "\n",
  "\n",
  "\n",
  "\n",
  "\n",
  "FLAG{Y0U_C@NT_S33_M3}\033[2K\n",  //is immediately cleared because of ANSI escape code. Requires serial logging.
  "\n",
  "\n",
  "\n",
  "\n",
  "\n",
  "FLJ42   TK01   BM90   R601   6J82   FP03   ZWO::   JW89                       \n",
  "DEF TRPCON: 43.45342.349                                                      \n",
  "\033[7mCPU AUTH RY-345-AX3     SYSCOMP STATUS: ALL PORTS ACTIVE                  \033[0m\n",
  "(311) 936-2364                                                                 \n",
  "**********************************************************************        \n",
  "1105-45-F6-B456                 NOPR STATUS: TRAK OFF   PRON ACTIVE          \n",
  "\033[2J\033[H                                                                 \n",
};

void wopr_display_glitch(int digit) {  //display digit with random segments enabled
  uint16_t rand;
  for (int i = 3; i >= digit; i--) {
    rand = random(0b100000000000000);
    display.illuminateChar(rand, i);
  }
  //display.updateDisplay();
}

void modem_glitch() {  //glitch display 'WOPR' with serial code printed
  char wopr[4] = { 'W', 'O', 'P', 'R' };
  int resetLine = 0;
  int lineNo = 0;
  int weird_code_len = sizeof(weird_code) / sizeof(weird_code[0]);
  for (int digit = 0; digit < 4; digit++) {
    display.setBrightness(15); //bright display while glitching
    long time = 1500;
    unsigned long start = millis();
    while (millis() - start < time) {
      display.clear();
      wopr_display_glitch(digit);
      for (int i = 0; i < digit; i++) { //every 1.5s, show new 'WOPR' letter
        display.printChar(wopr[i], i);
      }
      display.updateDisplay();
      if (random(0, 2) == 0) { //randomly print code to serial
        lineNo++;
        SerialSAO.print("\033[0G");
        SerialSAO.print(weird_code[lineNo % weird_code_len]);
      } else {
        delay(30);
      }
      if (lineNo % 30 == 0) { clear(); }  //clear screen every 30 lines printed
    }
    display.clear();
    for (int i = 0; i < 4; i++) {
      display.printChar(wopr[i], i);
    }
    display.updateDisplay();
  }
  display.setBrightness(4); //return to low display
  display.updateDisplay();
}

bool checkInput(String text) {
  return String(receivedChars).equalsIgnoreCase(text);
}

void receive_handle() {
  String input = String(receivedChars);
  input.toUpperCase();
  if (input.equals("HELP LOGON")) {
    println("HELP NOT AVAILABLE");
  } else if (input.equals("HELP GAMES")) {
    println("'GAMES' REFERS TO MODELS, SIMLUATIONS, AND GAMES WHICH HAVE TACTICAL AND STRATEGIC APPLICATIONS.");
  } else if (input.equals("LIST GAMES")) {
    for (int i = 0; i < gameCount - 1; i++) {
      println(gameList[i]);
      delay(500);
      if (i == 13) {  //newline and pause before global thermonuclear war
        println();
        delay(1000);
      }
    }
  } else if (input.equals("EXIT") || input.equals("QUIT")) {
    exitState = true;
  } else if (!authenticated) {
    println("IDENTIFICATION NOT RECOGNIZED BY SYSTEM");
  } else if (authenticated) {
    if (input.equals("HELP")) {
      println("VALID COMMANDS: LIST GAMES, QUIT, PLAY, FLAG");
    } else if (input.equals("FLAG")) {
      println("CONGRATULATIONS ON CONNECTING TO WOPR!");
      uint8_t flag[32] = { 0x24, 0x1f, 0x07, 0x9a, 0x9b, 0x68, 0x71, 0x58, 0x9d, 0xce, 0xd7, 0xe9, 0x6a, 0x42, 0x51, 0xdf, 0x83, 0xbc, 0x1a, 0x0e, 0xc6, 0x91, 0x20, 0xfd, 0xc1, 0x23, 0xd8, 0xcc, 0x29, 0xc1, 0x45, 0x68 };
      println(flagDecrypt(flag));
    } else if (input.indexOf("PLAY") != -1) {
      bool played = false;
      for (int i = 0; i < gameCount; i++) {
        if (input.indexOf(gameList[i]) != -1) {
          played = true;
          print("LOADING " + gameList[i]);
          for (byte x = 0; x < 3; x++) {
            delay(500);
            print(".");
          }
          (*gameFuncList[i])(); //executes game function
        }
      }
      if (!played) {
        println("GAME NOT FOUND");
      }
    } else {
      println("INVALID COMMAND. TYPE `HELP` TO LIST COMMANDS.");
    }
  }
  println();
}
