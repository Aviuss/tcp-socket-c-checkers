#include "checkers.h"
#include "checkersInputParsing.h"
#include <stdio.h>
#include <stdlib.h>

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

    char opponent = 'b';
    if (game->turn == 'b') {
        opponent = 'w';
    }

    struct CheckersCoords currentCoords = sequence.seq[0];
    if (checkersCoordsInBounds(&currentCoords) == -1 || game->board[currentCoords.in][currentCoords.ia] != colorMove) {
        free(sequence.seq); sequence.seq = NULL; return -1;
    }

    int moved = -1;

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
            moved = 1;
            currentCoords = nextCoords;
        }
    }

    // strike
    if (moved == -1) {
        for (int i = 1; i < sequence.seqLen; i++) {
            struct CheckersCoords nextCoords = sequence.seq[i];
            if (checkersCoordsInBounds(&nextCoords) == -1 || game->board[nextCoords.in][nextCoords.ia] != ' ' || game->board[(currentCoords.in + nextCoords.in)/2][(currentCoords.ia + nextCoords.ia)/2] != opponent) {
                free(sequence.seq); sequence.seq = NULL; return -1;
            }

            int increment = 2;
            if (colorMove == 'b') { increment = -2; }

            if (
                (currentCoords.in + increment == nextCoords.in &&
                currentCoords.ia - increment == nextCoords.ia) // left up
                ||
                (currentCoords.in + increment == nextCoords.in &&
                currentCoords.ia + increment == nextCoords.ia) // right up
                ||
                (currentCoords.in - increment == nextCoords.in &&
                currentCoords.ia - increment == nextCoords.ia) // left down
                ||
                (currentCoords.in - increment == nextCoords.in &&
                currentCoords.ia + increment == nextCoords.ia) // right down
            ) {
                game->board[currentCoords.in][currentCoords.ia] = ' ';
                game->board[nextCoords.in][nextCoords.ia] = colorMove;
                game->board[(currentCoords.in + nextCoords.in)/2][(currentCoords.ia + nextCoords.ia)/2] = ' ';
                moved = 1;
                currentCoords = nextCoords;
            } else if (i + 1 < sequence.seqLen) {
                free(sequence.seq); sequence.seq = NULL; return -1;
            }
        }
    
    }   

    if (moved == -1) {
        free(sequence.seq); sequence.seq = NULL;
        return -1;
    }
    
    free(sequence.seq); sequence.seq = NULL;
    game->turn = opponent;
    return 1;
}

char getResult(struct Game* game) {
    return ' ';
}