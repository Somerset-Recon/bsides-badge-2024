void mapPrint() {
  clear();
  println("", 2, 6);
  println("     ____________/\\'--\\__         __                       ___/-\\             ", 2, 6);
  println("   _/                   \\     __/  |          _     ___--/      / __          ", 2, 6);
  println("  /                      |   /    /          / \\__--           /_/  \\/---\\    ", 2, 6);
  println("  |                       \\_/    /           \\                            \\   ", 2, 6);
  println("  |'                            /             |                            |  ", 2, 6);
  println("   \\                           |            /^                             /  ", 2, 6);
  println("    \\__                       /            |                          /---/   ", 2, 6);
  println("       \\__                   /              \\              ___    __  \\       ", 2, 6);
  println("          \\__     ___    ___ \\               \\_           /   \\__/  /_/       ", 2, 6);
  println("              \\  /    \\_/   \\ \\                \\__'-\\    /                    ", 2, 6);
  println("               \\/            \\/                      \\__/                     ", 2, 6);
  println("", 2, 6);
  println("          UNITED STATES                               SOVIET UNION\n", 2, 6);
}

int choose_side() {
  int choice = -1;
  println("WHICH SIDE DO YOU WANT?");
  println();
  println("  1. UNITED STATES");
  println("  2. SOVIET UNION\n");
  while (choice != 0 && choice != 1 && choice != 2) {
    print("PLEASE CHOOSE ONE (OR 0 TO QUIT): ");
    receive();
    choice = receivedChars[0] - '0';
  }
  return choice;
}

int32_t getMissileCount() {
  long long missileCount = 0;
  char *ptr = NULL;
  while (missileCount <= 0) {
    print("Enter how many missiles to launch: ");
    empty_rx();
    receive();
    missileCount = strtoll(receivedChars, &ptr, 10);
    if (missileCount <= 0) {
      println("MUST PROVIDE POSITIVE INTEGER.");
    } else if (missileCount > INT32_MAX) {
      println("ERROR: BUDGET NOT FOUND.");
      missileCount = -1;
    }
  }
  return int32_t(missileCount); //converts long long to 32 bit int
}

void GTW() {
  mapPrint();
  String sides[2] = { "UNITED STATES", "SOVIET UNION" };
  int choice = choose_side();
  if (choice == 0) { return; }
  print("YOU HAVE SELECTED: ");
  println(sides[choice - 1]);
  println();
  delay(1000);
  println("\033[4mAWAITING FIRST STRIKE COMMAND\033[24m\n");
  int missileCount = getMissileCount();
  int missileCountEnemy = missileCount + 1;
  println();
  int missileArr[2] = { 0, 0 };
  if (choice == 1) {
    missileArr[0] = missileCount;
    missileArr[1] = missileCountEnemy;
  } else {
    missileArr[1] = missileCount;
    missileArr[0] = missileCountEnemy;
  }
  mapPrint();
  println("UNITED STATES MISSLES LAUNCHED: " + String(missileArr[0]));
  println("SOVIET UNION MISSILES LAUNCHED: " + String(missileArr[1]));

  SerialSAO.print("\033[?25l");  //hide cursor
  for (int i = 0; i < 13; i++) { //does a missile launch animation
    if (missileArr[0] > 0) { //only launches if missle count > 0
      print("\033[31m");
      gotoxy(10 + i, 6 - i);
      if (i == 0) {
        print("/\\");
      } else if (i <= 6) {
        print("/");
      } else if (i > 6 && i != 12) {
        gotoxy(40 + i, i - 6);
        print("\\");
      } else {
        gotoxy(40 + i, i - 6);
        print("X");
      }
    }
    if (missileArr[1] > 0) { //only launches if missle count > 0
      print("\033[31m");
      gotoxy(62 - i, 6 - i);
      if (i == 0) {
        print("/\\");
      } else if (i <= 6) {
        gotoxy(62 - i + 1, 6 - i);
        print("\\");
      } else if (i > 6 && i != 12) {
        gotoxy(30 - i, i - 6);
        print("/");
      } else {
        gotoxy(30 - i, i - 6);
        print("X");
      }
    }
  }
  print("\033[0m");
  if (missileCountEnemy <= 0) { //WIN
    gotoxy(0, 18);
    uint8_t flag[32] = { 0x14, 0x73, 0x80, 0xfe, 0xa1, 0xe2, 0x76, 0xcf, 0x93, 0x55, 0x5a, 0x38, 0x44, 0xdc, 0xd3, 0x61, 0x59, 0x9a, 0x0c, 0x88, 0x78, 0xcd, 0x7c, 0x69, 0x7a, 0x10, 0x58, 0x13, 0xb6, 0x84, 0x0d, 0x50 };
    println(flagDecrypt(flag));
  } else { //LOSE
    gotoxy(0, 18);
    print("LOST. PLAY AGAIN? (y/n): ");
    receive();
    if (receivedChars[0] == 'y' || receivedChars[0] == 'Y') {
      GTW();
    } else {
      println("THANK YOU FOR PLAYING");
      return;
    }
  }
}
