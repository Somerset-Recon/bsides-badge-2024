
String remarkList[] = {
  "\r\nI COULD NOT BE BOTHERED TO LEARN THE RULES TO 'HEARTS'.\r\n",
  "\r\nIT SEEMS THE ONLY WINNING MOVE IS NOT TO PLAY... HEARTS.\r\n",
  "\r\nRULESET NOT FOUND\r\n",
  "\r\nI'D BE TENDER, I'D BE GENTLE, AND AWFUL SENTIMENTAL IF I ONLY HAD A HEART.\r\n"
};

void randomRemark() {
  print(remarkList[random(0, sizeof(remarkList) / sizeof(remarkList[0]))]);
}

void printHearts(){
  SerialSAO.println("            .;;;, .;;;,                   .;;;, .;;;,           \r\n"
"       .;;;,;;;;;,;;;;;,.;;;,       .;;;.,;;;;;,;;;;;,;;;.      \r\n"
"      ;;;;xXXxXXxXXxXXxXXx;;;. .,. .;;;xXXxXXxXXxXXxXX;;;;;     \r\n"
"  .,,.`xXX'             `xXXx,;;;;;,xXXx'            `XXx;;,,.  \r\n"
" ;;;;xXX'                  `xXXx;xXXx'                 `XXx;;;; \r\n"
" `;;XX'                       `XXX'                      `XX;;' \r\n"
",;;,XX                         `X'                        XX,;;,\r\n"
";;;;XX,                                                  ,XX;;;;\r\n"
" ``.;XX,                                                ,XX;,'' \r\n"
"   ;;;;XX,                                            ,XX;;;;   \r\n"
"    ```.;XX,                                        ,XX;,'''    \r\n"
"       ;;;;XX,                                    ,XX;;;;       \r\n"
"        ```,;XX,                                ,XX;,'''        \r\n"
"            ;;;;XX,                          ,XX;;;;            \r\n"
"             ````,;XX,                    ,XX;, '''             \r\n"
"                 ;;;;;XX,              ,XX;;;;                  \r\n"
"                  `````,;XX,        ,XX;,''''                   \r\n"
"                       ;;;;;XX,  ,XX;;;;;                       \r\n"
"                        `````;;XX;;'''''                        \r\n"
"                             `;;;;'                             \r\n\r\n\r\n");
}

void hearts() { //Arduino unpredictably orders buffers when compiling. Easier to provide vulnerable source code that has the same "vulnerability".
  char ruleset[8] = { "CHESS" };
  char userInput[28];
  bool good = false;
  clear();
  printHearts();
  println();
  while (true) {
    print("LOADING RULESET HEARTS");
    for (byte x = 0; x < 3; x++) {
      delay(250);
      print(".");
    }
    if (good) {
      println();
      println("RULESET FOUND!");
      uint8_t flag[] = { 0x66, 0x4e, 0x55, 0x65, 0x2b, 0x64, 0x0b, 0xba, 0x2c, 0x9c, 0xda, 0x3e, 0xd6, 0x0b, 0x50, 0x79, 0x16, 0x57, 0xb0, 0x0a, 0x65, 0x16, 0x2a, 0x83, 0xa5, 0x3e, 0x9f, 0x85, 0x6a, 0x2f, 0x50, 0xa9 };
      println(flagDecrypt(flag));
      return;
    } else {
      randomRemark();
      print("HOW ABOUT A NICE GAME OF CHESS? (y/n/e): ");
      receive();
      memset(userInput, 0, 28);
      strncpy(userInput, receivedChars, 28);
      if (userInput[0] == 'y' || userInput[0] == 'Y') { //play chess
        chess();
        return;
      } else if (userInput[0] == 'e' || userInput[0] == 'E') { //exit
        return;
      } else if (userInput[20] == 'H' && userInput[21] == 'E' && userInput[22] == 'A' && userInput[23] == 'R' && userInput[24] == 'T' && userInput[25] == 'S' && userInput[26] == '\0' && userInput[27] == '\0') {
        //"overflows" the userInput buffer into ruleset
        strcpy(ruleset, "HEARTS");
        good = true;
      }
    }
  }
  return;
}