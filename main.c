#include <stdio.h>
#include "checkers.h"
#include <string.h>

int main() {
    struct Game game;
    initGame(&game);
    game.board[2][0] = 'W';

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
