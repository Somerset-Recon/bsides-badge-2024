void ATGA() {
  println("WELCOME TO AIR-TO-GROUND ACTIONS.");
  println("AS A GROUND TRAFFIC CONTROLLER, YOU HAVE A RESPONSIBILITY TO LISTEN TO ANY AND ALL AIR TRAFFIC REACHING YOUR STATION.");
  println("BUT EVERYTHING IS QUIET AND ALL IS WELL");
  int d = 500;
  for (byte x = 0; x < 5; x++) {
    delay(d);
    d += 500;
    print(".");
  }
  delay(d);
  clear();
  SerialSAO.println("               \ /                                          \   /\r\n"
"              --o--           `\\             //'      .____-/.\-____.\r\n"
"                                \\           //             ~`-'~\r\n"
"                                 \\. __-__ .//\r\n"
"                       ___/-_.-.__`/~     ~\'__.-._-\___                    \r\n"
".|.       ___________.'__/__ ~-[ \.\'-----'/./ ]-~ __\__`.___________       .|.\r\n"
"~o~~~~~~~--------______-~~~~~-_/_/ |   .   | \_\_-~~~~~-______--------~~~~~~~o~\r\n"
"' `               + + +  (X)(X)  ~--\__ __/--~  (X)(X)  + + +               ' `\r\n"
"                             (X) `/.\' ~ `/.\' (X)  \r\n"
"                                 \"\_/\"   \"\_/\"\r\n\r\n\r\n"
);
  println("YOU ARE VIOLENTLY WOKEN UP TO THE SOUND OF FIGHTER JETS. HOW COULD YOU FALL ASLEEP ON THE JOB?!");
  println("YOU MANAGE TO SUCCESSFULLY RECEIVE AN ENCRYPTED MESSAGE FROM THE FIGHTER JETS BEFORE THEY FLY AWAY.");
  println("105B576B1C731184A9066D7D2E818D378CEB264B40E2FA7A84F532B821AC8D41");
}