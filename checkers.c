#include "checkers.h"
#include <stdio.h>
#include <stdlib.h>

// {ia,in = -1} == undefined
struct CheckersCoords {
    int ia;
    int in;
};

// {seqLen = -1} == undefined
struct CheckersCoordsSequence {
    struct CheckersCoords* seq;
    int seqLen;
};

int checkersCoordsInBounds(struct CheckersCoords* coords) {
    if (coords->ia < 0 || coords->ia > 7 || coords->in < 0 || coords->in > 7) {
        return -1;
    }
    return 1;
}

struct CheckersCoords charPosToCoords(char* charPos, int charPosLen) {
    struct CheckersCoords coords;

    if (charPosLen != 2) {
        coords.ia = -1; coords.in = -1;
        return coords;
    }

    coords.ia = charPos[0] - 'A';
    coords.in = charPos[1] - '1';

    if (checkersCoordsInBounds(&coords) == -1) {
        coords.ia = -1; coords.in = -1;
        return coords;
    }

    return coords;
}

struct CheckersCoordsSequence commandToCoordsSequence(char* command, int commandCharLen) {
    struct CheckersCoordsSequence sequence;
    sequence.seq = NULL;
    sequence.seqLen = (commandCharLen+2)/4;

    if (commandCharLen != 4*sequence.seqLen - 2) {
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
        if (sequence.seq[i].ia == -1) {
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
    for (int in = 0; in < 8; in++) {
        for (int ia = 0; ia < 8; ia++) {
            if (ia % 2 != in % 2) {
                game->board[in][ia] = 'x';
                continue;
            }

            if (in < 3) {
                game->board[in][ia] = 'w';
            } else if (in > 4) {
                game->board[in][ia] = 'b';
            } else {
                game->board[in][ia] = ' ';
            }

        }
    }
}

void printBoard(struct Game* game) {
    printf("    A B C D E F G H\n");
    printf("  +-----------------+\n");
    for(int in = 7; in >= 0; in--) {
        printf("%d | ", in+1);
        for(int ia = 0; ia < 8; ia++) {
            if (game->board[in][ia] == 'x') {
                printf("  ");
            } else {
                printf("%c ", game->board[in][ia]);
            }
        }
        printf("| %d", in+1);
        printf("\n");
    }
    
    printf("  +-----------------+\n");
    printf("    A B C D E F G H\n");

    printf("Turn: %c\n\n", game->turn);    
}

int makeMove(
    struct Game* game,
    char colorMove,
    char* command, int commandCharLen
) {
    if (game->turn != colorMove) { return -1; }

    struct CheckersCoordsSequence sequence = commandToCoordsSequence(command, commandCharLen);
    if (sequence.seqLen <= 1) { return -1; }

    struct CheckersCoords currentCoords = sequence.seq[0];
    if (checkersCoordsInBounds(&currentCoords) == -1 || game->board[currentCoords.in][currentCoords.ia] != colorMove) {
        free(sequence.seq); sequence.seq = NULL; return -1;
    }

    if (sequence.seqLen == 2) { //simple move
        struct CheckersCoords nextCoords = sequence.seq[1];
        if (checkersCoordsInBounds(&nextCoords) == -1 || game->board[nextCoords.in][nextCoords.ia] != ' ') {
            free(sequence.seq); sequence.seq = NULL; return -1;
        }

        int increment = 1;
        if (colorMove == 'b') { increment = -1; }

        if (
            (currentCoords.in + increment == nextCoords.in &&
            currentCoords.ia - increment == nextCoords.ia) // left
            ||
            (currentCoords.in + increment == nextCoords.in &&
            currentCoords.ia + increment == nextCoords.ia) // right
        ) {
            game->board[currentCoords.in][currentCoords.ia] = ' ';
            game->board[nextCoords.in][nextCoords.ia] = colorMove;  
        } else  {
            free(sequence.seq); sequence.seq = NULL; return -1;
        }
    }

    if (game->turn == 'w') {
        game->turn = 'b';
    } else {
        game->turn = 'w';
    }
    
    free(sequence.seq); sequence.seq = NULL;
    return 1;
}

char getResult(struct Game* game) {
    return ' ';
}