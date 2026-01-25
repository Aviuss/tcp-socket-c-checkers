#ifndef CHECKERS_H
#define CHECKERS_H

// 'w', 'W' / 'b', 'B' ; white / black
// uppercase => queen
// ' ' - blank
// 'x' - invalid
struct Game {
    char board[8][8]; // [0][0] - lower right corner (A1) ; [in][ia] in => 1,2..., ia => A,B,...
    char turn;
    char won;
};

void initGame(struct Game* game);
void initEndGame(struct Game* game);


// return:
//       1 => move valid
//      -1 => move invalid
int makeMove(
    struct Game* game,
    char colorMove, // 'w' / 'b'
    char* command, int commandCharLen // "A1->B3"
);

void printBoard(struct Game* game);

#endif