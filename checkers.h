#pragma once

// 'w', 'W' / 'b', 'B' ; white / black
// uppercase => queen
// ' ' - blank
// 'x' - invalid
struct Game {
    char board[8][8]; // [0][0] - lower right corner (A1) ; [y][x] y => A,B..., x => 1,2,...
    char turn;
};

void initGame(struct Game* game);

// return:
//       1 => move valid
//      -1 => move invalid
int makeMove(
    struct Game* game,
    char colorMove, // 'w' / 'b'
    char* fromPos, int fromPosLen,
    char* toPos, int toPosLen
);

// return:
//      'w' / 'b' / ' ' ; white / black / none
char getResult(struct Game* game);

void printBoard(struct Game* game);