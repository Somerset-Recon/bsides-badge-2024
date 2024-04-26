#define SIZE 3
char board[SIZE][SIZE];
int textOffset = 22;

int getUsers() {
  println("ONE OR TWO PLAYERS?");
  print("PLEASE LIST NUMBER OF PLAYERS: ");
  while (true) {
    receive();
    if (String(receivedChars) == "ONE") {
      return 1;
    } else if (String(receivedChars) == "TWO") {
      return 2;
    } else if (String(receivedChars) == "ZERO") {
      return 0;
    } else if (String(receivedChars).equalsIgnoreCase("exit")) {
      return -99;
    } else {
      println("INVALID SELECTION. PLEASE SELECT 'ONE', 'TWO', OR 'EXIT'.");
    }
  }
}

void gotoxy(int x, int y) {
  SerialSAO.print("\033[" + String(y + 1) + ";" + String(x + 1) + "H");
}

void draw_nought(int row_offset, int col_offset) {
  char nought[7][14] = {
    { ' ', '#', '#', ' ' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { '#', ' ', ' ', '#' },
    { ' ', '#', '#', ' ' }
  };

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 4; j++) {
      gotoxy(col_offset + j, row_offset + i);
      SerialSAO.print("\033[34m");
      SerialSAO.print(nought[i][j]);
      SerialSAO.print("\033[0m");
    }
  }
}

void draw_cross(int row_offset, int col_offset) {
  char cross[7][14] = {
    { '#', ' ', ' ', '#' },
    { ' ', '#', '#', ' ' },
    { ' ', '#', '#', ' ' },
    { ' ', '#', '#', ' ' },
    { '#', ' ', ' ', '#' }
  };

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 4; j++) {
      gotoxy(col_offset + j, row_offset + i);
      SerialSAO.print("\033[31m");
      SerialSAO.print(cross[i][j]);
      SerialSAO.print("\033[0m");
    }
  }
}

void draw_grid() {
  int rows = 24, cols = 80;
  int cell_size = 7;  // Grid cell size
  int grid_size = 3;  // 3x3 grid
  int start_row = 0;
  int start_col = 0;

  for (int i = 0; i <= grid_size; i++) {
    gotoxy(start_col, start_row + i * cell_size);
    for (int j = 0; j < (cell_size * 2) * grid_size; j++) {
      SerialSAO.print("*");
    }
  }

  for (int i = 0; i <= grid_size; i++) {
    for (int j = 0; j < (cell_size * grid_size) + 1; j++) {
      gotoxy(start_col + i * (cell_size * 2), start_row + j);
      SerialSAO.print("*");
    }
  }

  char row_labels[] = { 'a', 'b', 'c' };
  for (int row = 0; row < grid_size; row++) {
    for (int col = 1; col <= grid_size; col++) {
      int x = start_col + (col - 1) * (cell_size * 2) + ((cell_size * 2) / 2);
      int y = start_row + row * cell_size + cell_size - 1;
      gotoxy(x, y);
      SerialSAO.print(row_labels[row]);
      SerialSAO.print(col);
    }
  }
}

void reset_board() {
  clear();
  draw_grid();
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      board[i][j] = '\0';
    }
  }
}

void draw_symbol(int row, int col, char symbol) {
  int cell_size = 7;  // Grid cell size
  int grid_size = 3;  // 3x3 grid
  int rows = 24, cols = 80;
  int start_row = 0;
  int start_col = 22;
  int x = (start_col + col * (cell_size * 2) - (cell_size * 2)) - 2;
  int y = start_row + row * cell_size + (cell_size / 2) - 2;
  char command[200];

  if (symbol == 'O') {
    draw_nought(y, x);
  } else {
    draw_cross(y, x);
  }
}

char get_side_choice() {
  char side;
  while (side != 'X' && side != 'O' && side != '_') {
    println("X OR O?");
    receive();
    side = toupper(receivedChars[0]);
    if (side != 'X' && side != 'O' && side != '_') {
      println("INVALID OPTION. SELECT 'X', 'O', OR `_` TO EXIT.");
    }
  }
  return side;
}


void get_player_choice(int *row, int *col, char side) {
  gotoxy(0, textOffset + 1);
  SerialSAO.print("SELECT SQUARE (" + String(side) + "):\n");
  gotoxy(0, textOffset + 2);
  receive();
  char testRow = receivedChars[0];
  char testCol = receivedChars[1];
  while (testRow < 'a' || testRow > 'c' || testCol < '1' || testCol > '3' || board[testRow - 'a'][testCol - '0' - 1] != '\0') {
    gotoxy(0, textOffset + 1);
    SerialSAO.print("INVALID SQUARE     \n");
    SerialSAO.print("                   \n");
    delay(1000);
    gotoxy(0, textOffset + 1);
    SerialSAO.print("                   \n");
    SerialSAO.print("                   \n");
    gotoxy(0, textOffset + 1);
    SerialSAO.print("SELECT SQUARE (" + String(side) + "):\n");
    gotoxy(0, textOffset + 2);
    receive();
    testRow = receivedChars[0];
    testCol = receivedChars[1];
  }
  *row = testRow - 'a';
  *col = testCol - '0' - 1;
  gotoxy(0, textOffset + 1);
  SerialSAO.print("                   \n");
  SerialSAO.print("                   \n");
}

int minimax(char board[SIZE][SIZE], int depth, bool isMax, char computerSide, char playerSide) { //computer "ai" to solve tic-tac-toe
  char winner = '\0';
  if (check_winner(computerSide)) return +10 - depth;
  if (check_winner(playerSide)) return -10 + depth;
  if (check_draw()) return 0;

  if (isMax) {
    int bestVal = -1000;
    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        if (board[i][j] == '\0') {
          board[i][j] = computerSide;
          int value = minimax(board, depth + 1, !isMax, computerSide, playerSide);
          bestVal = (value > bestVal) ? value : bestVal;
          board[i][j] = '\0';  // Undo move
        }
      }
    }
    return bestVal;
  } else {
    int bestVal = 1000;
    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        if (board[i][j] == '\0') {
          board[i][j] = playerSide;
          int value = minimax(board, depth + 1, !isMax, computerSide, playerSide);
          bestVal = (value < bestVal) ? value : bestVal;
          board[i][j] = '\0';  // Undo move
        }
      }
    }
    return bestVal;
  }
}

void get_computer_choice(int *row, int *col, char side) {
  int bestVal = -1000;
  int moveRow = -1;
  int moveCol = -1;
  char playerSide = (side == 'X') ? 'O' : 'X';

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (board[i][j] == '\0') {
        board[i][j] = side;
        int moveVal = minimax(board, 0, false, side, playerSide);
        board[i][j] = '\0';  // Undo move
        if (moveVal > bestVal) {
          moveRow = i;
          moveCol = j;
          bestVal = moveVal;
        }
      }
    }
  }
  *row = moveRow;
  *col = moveCol;
}

bool check_winner(char symbol) {
  // Check rows
  for (int i = 0; i < SIZE; i++) {
    if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) {
      return true;
    }
  }
  // Check columns
  for (int i = 0; i < SIZE; i++) {
    if (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol) {
      return true;
    }
  }
  // Check diagonals
  if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) {
    return true;
  }
  if (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) {
    return true;
  }
  return false;
}

bool check_draw() {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (board[i][j] == '\0') {
        return false;  // If any square is empty, it's not a draw
      }
    }
  }
  return true;  // All squares are filled without a winner
}

void tictactoe() {
  clear();
  int playerCount = getUsers();
  if (playerCount == -99) {
    clear();
    return;
  }
  char player1 = 'X';
  if (playerCount == 1) {
    player1 = get_side_choice();
  }
  if (player1 == '_') {
    clear();
    return;
  }
  reset_board();
  char player2 = (player1 == 'X') ? 'O' : 'X';
  if (playerCount == 2 || playerCount == 1) {
    int turn = 0;
    while (true) {
      int row, col;
      char side = (turn == 0) ? 'X' : 'O';
      if (side == player2 && playerCount == 1) {
        delay(random(500, 800));
        get_computer_choice(&row, &col, side);
      } else {
        get_player_choice(&row, &col, side);
      }
      gotoxy(0, textOffset + 2);
      SerialSAO.print("                                  ");  // Print spaces over the previous prompt
      board[row][col] = side;
      draw_symbol(row, col, side);
      if (check_winner(side)) {
        gotoxy(0, textOffset + 1);
        if (playerCount == 2) {
          print("PLAYER #" + (String(turn + 1)) + " WINS!\n");
        } else {
          print("COMPUTER WINS!\n");
        }
        break;
      }
      if (check_draw()) {
        gotoxy(0, textOffset + 1);
        print("STALEMATE.\n");
        break;
      }
      turn = 1 - turn;  // Alternate between players
    }
    char playagain;
    gotoxy(0, textOffset + 3);
    print("WANT TO PLAY AGAIN? (Y/N): ");
    receive();
    playagain = receivedChars[0];
    if (playagain == 'Y' || playagain == 'y') {
      tictactoe();
    } else {
      println("THANK YOU FOR PLAYING.");
    }
  } else { //player ZERO
    SerialSAO.print("\033[?25l"); //hide cursor
    int randomTime = 600;
    int turn = 0;
    int gameCount = 0;
    bool start = true;
    delay(randomTime);
    char sides[2] = { 'X', 'O' };
    while (randomTime > 1 || gameCount < 50) {
      int row, col;
      if (start == true) { //begin with a random position
        int randomRow = random(0, 3);  // random number between 0 and 2
        int randomCol = random(0, 3);  // random number between 0 and 2
        board[randomRow][randomCol] = sides[turn];
        draw_symbol(randomRow, randomCol, sides[turn]);
        delay(randomTime);
        turn = 1 - turn;
        start = false;
      }
      char side = sides[turn];
      get_computer_choice(&row, &col, side);
      board[row][col] = side;
      draw_symbol(row, col, side);
      if (check_draw()) {
        gotoxy(0, textOffset + 1);
        SerialSAO.print("STALEMATE.\n");
        randomTime -= randomTime / 4; //decrease delay by 25% each time
        if (randomTime < 4 && randomTime != 0) {
          randomTime--;
        }
        gameCount++;
        delay(randomTime);
        reset_board();
        SerialSAO.print("\033[?25l"); //hide cursor
        turn = 0;
        start = true;
      } else {
        delay(randomTime);
        turn = 1 - turn;  // Alternate between players
      }
    }
    SerialSAO.print("\033[?25h"); //shows cursor (not necessary because of clear())
    clear();
    delay(4000);
    println("A STRANGE GAME.");
    delay(4000);
    println("THE ONLY WINNING MOVE IS NOT TO PLAY.");
    delay(1000);
    println();
    println("HOW ABOUT A NICE GAME OF CHESS?");
    println();
    delay(1000);
    uint8_t flag[32] = {0x2b,0xd3,0xb3,0xf2,0x47,0xf1,0x2a,0xcd,0xfd,0xc0,0x76,0x5f,0x94,0x22,0x4f,0x76,0x97,0xda,0xea,0xaa,0xaf,0x3f,0xc4,0xaf,0xe3,0x0c,0x30,0xd3,0x70,0x13,0x01,0x68};
    println(flagDecrypt(flag));
    receive();
  }
}