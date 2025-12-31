#include "checkers.h"
#include <stdio.h>
#include <stdlib.h>

void initGame(struct Game* game) {
    game->turn = 'w';
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (x % 2 != y % 2) {
                game->board[y][x] = 'x';
                continue;
            }

            if (y < 3) {
                game->board[y][x] = 'w';
            } else if (y > 4) {
                game->board[y][x] = 'b';
            } else {
                game->board[y][x] = ' ';
            }

        }
    }
}

void printBoard(struct Game* game) {
    printf("  A B C D E F G H\n");
    for(int y = 7; y >= 0; y--) {
        printf("%d ", y);
        for(int x = 0; x < 8; x++) {
            if (game->board[y][x] == 'x') {
                printf("  ");
            } else {
                printf("%c ", game->board[y][x]);
            }
        }
        printf("%d", y);
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