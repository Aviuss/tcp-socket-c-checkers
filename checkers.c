#include "checkers.h"
#include <stdio.h>
#include <stdlib.h>

// {x,y = -1} == undefined
struct CheckersCoords {
    int x;
    int y;
};

// {seqLen = -1} == undefined
struct CheckersCoordsSequence {
    struct CheckersCoords* seq;
    int seqLen;
};

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

struct CheckersCoordsSequence commandToCoordsSequence(char* command, int commandLen) {
    struct CheckersCoordsSequence sequence;
    sequence.seq = NULL;
    sequence.seqLen = (commandLen+2)/4;

    if (commandLen != 4*sequence.seqLen - 2) {
        sequence.seqLen = -1;
        return sequence;
    }

    sequence.seq = malloc(sequence.seqLen * sizeof(struct CheckersCoords));

    char charPos[2];
    for (int i = 0; i < sequence.seqLen; i++)
    {
        charPos[0] = command[4*i];
        charPos[1] = command[4*i+1];

        sequence.seq[i] = charPosToCoords(charPos, 2);
        if (sequence.seq[i].x == -1) {
            free(sequence.seq);
            sequence.seq = NULL;
            sequence.seqLen = -1;
            return sequence;
        }
    }
    
    return sequence;
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


    char a[6] = {'A', '1', '-', '>', 'B', '3'};
    struct CheckersCoordsSequence x = commandToCoordsSequence(a, 6);
    printf("%d \n", x.seqLen);
    for (int i = 0; i < x.seqLen; i++)
    {
        printf("%d %d \n", x.seq[i].x, x.seq[i].y);
    }
    
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