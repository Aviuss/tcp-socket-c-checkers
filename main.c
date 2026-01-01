#include <stdio.h>
#include "checkers.h"
#include <string.h>

int main() {
    struct Game game;
    initGame(&game);
    //game.board[2][0] = 'W';
    //game.board[3][1] = 'b';
    game.board[6][4] = ' ';
    game.board[7][5] = ' ';
    game.board[5][3] = ' ';

    while (getResult(&game) == ' ')
    {
        printBoard(&game);

        char buf[255];
        memset(buf, 0, sizeof buf);
        fgets(buf, sizeof buf, stdin);

        makeMove(&game, game.turn, buf, sizeof buf);
    }
    
    return 0;
}
