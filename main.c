#include <stdio.h>
#include "checkers.h"

int main() {
    struct Game game;
    initGame(&game);
    printBoard(&game);

    char a[] = "A3->B4";
    makeMove(&game, 'w', a, sizeof a);
    printBoard(&game);

    char a2[] = "       B6->C5";
    makeMove(&game, 'b', a2, sizeof a2);
    printBoard(&game);

    char a3[] = "G3->H4      ";
    makeMove(&game, 'w', a3, sizeof a3);
    printBoard(&game);

    char a4[] = " C5->A3  ";
    makeMove(&game, 'b', a4, sizeof a4);
    printBoard(&game);

    return 0;
}
