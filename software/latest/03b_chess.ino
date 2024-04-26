String chessRemark[] = {
  "\r\nWOULDN'T YOU PREFER A NICE GAME OF CHESS?\r\n",
  "\r\nGAME NOT FOUND\r\n"
  "\r\nIT SEEMS THE ONLY WINNING MOVE IS NOT TO PLAY.\r\n",
  "\r\nCHESS IS A MUCH BETTER GAME.\r\n",
  "\r\nNO, I DON'T THINK WE WILL BE PLAYING THIS GAME\r\n",
  "\r\nTHIS IS NOT THE GAME YOU ARE LOOKING FOR\r\n",
};

void chesscheck(){
  print(chessRemark[random(0, sizeof(chessRemark) / sizeof(chessRemark[0]))]);
}

void chess() { 
  println("ERROR: CHESS NOT FOUND"); //XD
}