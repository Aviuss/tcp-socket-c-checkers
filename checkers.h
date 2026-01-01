#pragma once

// 'w', 'W' / 'b', 'B' ; white / black
// uppercase => queen
// ' ' - blank
// 'x' - invalid
struct Game {
    char board[8][8]; // [0][0] - lower right corner (A1) ; [in][ia] in => 1,2..., ia => A,B,...
    char turn;
};

void initGame(struct Game* game);

// return:
//       1 => move valid
//      -1 => move invalid
int makeMove(
    struct Game* game,
    char colorMove, // 'w' / 'b'
    char* command, int commandCharLen // "A1->B3"
);

// return:
//      'w' / 'b' / ' ' ; white / black / none
char getResult(struct Game* game);

void printBoard(struct Game* game);