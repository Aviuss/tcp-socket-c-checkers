#include <stdio.h>
#include "checkers.h"

int main() {
    struct Game game;
    initGame(&game);
    printBoard(&game);

    char a[6] = {'A', '3', '-', '>', 'B', '4'};
    makeMove(&game, 'w', a, 6);
    printBoard(&game);

    return 0;
}
