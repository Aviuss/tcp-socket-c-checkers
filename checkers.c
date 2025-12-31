#include "checkers.h"
#include <stdio.h>
#include <stdlib.h>

void initGame(struct Game* game) {
    game->turn = 'w';
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            game->board[y][x] = 'x';
        }
    }
}

void printBoard(struct Game* game) {
    printf("  A B C D E F G H\n");
    for(int i = 0; i < 8; i++) {
        printf("%d ", 8-i);
        for(int j = 0; j < 8; j++) {
            printf("%c ", game->board[i][j]);
        }
        printf("%d", 8-i);
        printf("\n");
    }
    printf("  A B C D E F G H\n");

    printf("Turn: %c\n\n", game->turn);
}

int makeMove(
    struct Game* game,
    char colorMove,
    char* fromPos, int fromPosLen,
    char* toPos, int toPosLen
) {
    return 1;
}

char getResult(struct Game* game) {
    return ' ';
}