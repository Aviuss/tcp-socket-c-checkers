#include <stdio.h>
#include "checkers.h"

int main() {
    struct Game game;
    initGame(&game);
    printBoard(&game);

    char a[6] = {'A', '3', '-', '>', 'B', '4'};
    makeMove(&game, 'w', a, 6);
    printBoard(&game);

    char a2[6] = {'B', '6', '-', '>', 'C', '5'};
    makeMove(&game, 'b', a2, 6);
    printBoard(&game);

    char a3[6] = {'G', '3', '-', '>', 'H', '4'};
    makeMove(&game, 'w', a3, 6);
    printBoard(&game);

    char a4[6] = {'C', '5', '-', '>', 'A', '3'};
    makeMove(&game, 'b', a4, 6);
    printBoard(&game);

    return 0;
}
