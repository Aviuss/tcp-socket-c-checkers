#include "checkers.h"
#include <stdio.h>
#include <stdlib.h>

struct CheckersCoords {
    int x;
    int y;
};

// {x,y = -1} == undefined
struct CheckersCoords charPosToCoords(char* charPos, int charPosLen) {
    struct CheckersCoords coords;

    if (charPosLen != 2) {
        coords.x = -1; coords.y = -1;
        return coords;
    }

    coords.y = charPos[0] - 'A';
    coords.x = charPos[1] - '1';

    if (coords.x < 0 || coords.x > 7 || coords.y < 0 || coords.y > 7) {
        coords.x = -1; coords.y = -1;
        return coords;
    }

    return coords;
}

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
    printf("    A B C D E F G H\n");
    printf("  +-----------------+\n");
    for(int y = 7; y >= 0; y--) {
        printf("%d | ", y+1);
        for(int x = 0; x < 8; x++) {
            if (game->board[y][x] == 'x') {
                printf("  ");
            } else {
                printf("%c ", game->board[y][x]);
            }
        }
        printf("| %d", y+1);
        printf("\n");
    }
    
    printf("  +-----------------+\n");
    printf("    A B C D E F G H\n");

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