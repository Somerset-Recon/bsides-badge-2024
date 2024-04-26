void BJ() {
  clear();
  SerialSAO.print(" -------------       ------------- \r\n"
                  "|K♠           |     |A♣           |\r\n"
                  "|   -------   |     |   -------   |\r\n"
                  "|  |♠      |  |     |  |       |  |\r\n"
                  "|  |       |  |     |  |       |  |\r\n"
                  "|  |       |  |     |  |       |  |\r\n"
                  "|  |   K   |  |     |  |   ♣   |  |\r\n"
                  "|  |       |  |     |  |       |  |\r\n"
                  "|  |       |  |     |  |       |  |\r\n"
                  "|  |      ♠|  |     |  |       |  |\r\n"
                  "|   -------   |     |   -------   |\r\n"
                  "|           ♠K|     |           ♣A|\r\n"
                  " -------------       ------------- \r\n\r\n\r\n");
  println("WELCOME TO BLACKJACK.");
  println("YOU WILL TAKE THE NEXT SHIFT AS DEALER..");
  println("YOU WILL BE DEALING FOR 1337 GAMES. GIVEN THAT YOU WILL BE IMPERSONATING A COMPUTER, YOU ARE EXPECTED TO DEAL RAPIDLY.");
  println("WHEN PLAYERS PRESENT THEIR CARDS, IT IS YOUR JOB TO EITHER SAY 'LOSE', 'PUSH', OR 'WIN' FOR THE PLAYER'S HAND.");
  println("GOOD LUCK!");
  String cards = "A123456789TJQK";
  int cardVal[] = { 11, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10 };
  String answer;
  while (true) {
    for (int i = 1; i < 1338; i++) {
      SerialSAO.println("ROUND " + String(i));
      char cardone = random(0, cards.length() - 1);
      char cardtwo = random(0, cards.length() - 1);
      char cardthree = random(0, cards.length() - 1);
      char cardfour = random(0, cards.length() - 1);
      int dSum = cardVal[cardone] + cardVal[cardtwo];
      if (dSum > 21) {
        if (cardVal[cardone] == 11) {
          dSum -= 10;
        } else if (cardVal[cardtwo] == 11) {
          dSum -= 10;
        }
      }
      int pSum = cardVal[cardthree] + cardVal[cardfour];
      if (pSum > 21) {
        if (cardVal[cardthree] == 11) {
          pSum -= 10;
        } else if (cardVal[cardfour] == 11) {
          pSum -= 10;
        }
      }
      SerialSAO.println("DEALER'S HAND: " + String(cards[cardone]) + " " + String(cards[cardtwo]));
      SerialSAO.println("PLAYER'S HAND: " + String(cards[cardthree]) + " " + String(cards[cardfour]));

      if (pSum > 21) {
        answer = "LOSE";
      } else if (dSum > 21 && pSum <= 21) {
        answer = "LOSE";
      } else if (pSum > dSum) {
        answer = "WIN";
      } else if (dSum > pSum) {
        answer = "LOSE";
      } else if (pSum == dSum) {
        answer = "PUSH";
      }
      receive();
      if (String(receivedChars) == answer) {
        SerialSAO.println("CORRECT");
        SerialSAO.println();
      } else {
        println("INCORRECT. YOU ARE A POOR SUBSTITUTE FOR A COMPUTER. PLAY AGAIN? (Y/N): ");
        receive();
        if (receivedChars[0] == 'y' || receivedChars[0] == 'Y') {
          i = 0;
          continue;
        } else {
          println("THANK YOU FOR PLAYING.");
          return;
        }
      }
    }
    uint8_t flag[] = { 0x93, 0x6d, 0x7a, 0xf2, 0xe6, 0x0e, 0x11, 0x68, 0xde, 0x47, 0xcb, 0xa3, 0x3d, 0x25, 0xaf, 0x3a, 0xe8, 0x54, 0x93, 0x22, 0xa0, 0x97, 0x6b, 0x95, 0x8b, 0x3c, 0x21, 0xd0, 0xbd, 0x92, 0x70, 0x63 };
    println(flagDecrypt(flag));
    return;
  }
}