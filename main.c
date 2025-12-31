#include <stdio.h>
#include "checkers.h"

int main() {
    struct Game game;
    initGame(&game);
    printBoard(&game);

    return 0;
}
