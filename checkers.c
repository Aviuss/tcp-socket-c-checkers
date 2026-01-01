#include "checkers.h"
#include "checkersInputParsing.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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

    char opponent = 'b'; if (game->turn == 'b') { opponent = 'w'; }

    if (tolower(game->board[sequence.seq[0].in][sequence.seq[0].ia]) != colorMove) {
        free(sequence.seq); sequence.seq = NULL; return -1;
    }
    for (int i = 0; i < sequence.seqLen; i++) { // all moves in bounds
        if (checkersCoordsInBounds(&sequence.seq[i]) == -1) {
            free(sequence.seq); sequence.seq = NULL; return -1;
        }
    }

    int moved = -1;
    int striked = -1;
    int isQueen = -1;
    if ((colorMove == 'w' && game->board[sequence.seq[0].in][sequence.seq[0].ia] == 'W') ||
        (colorMove == 'b' && game->board[sequence.seq[0].in][sequence.seq[0].ia] == 'B')
    ) { isQueen = 1; }

    if (sequence.seqLen == 2) { //simple move
        struct CheckersCoords currentCoords = sequence.seq[0];
        struct CheckersCoords nextCoords = sequence.seq[1];
        if (game->board[nextCoords.in][nextCoords.ia] != ' ') {
            free(sequence.seq); sequence.seq = NULL; return -1;
        }

        struct CheckersCoords movementVector;
        movementVector.ia = nextCoords.ia - currentCoords.ia;
        movementVector.in = nextCoords.in - currentCoords.in;
        int vectorNorm = abs(movementVector.ia);

        if (abs(movementVector.ia) != abs(movementVector.in) || vectorNorm == 0 || (vectorNorm > 1 && isQueen == -1)) {
            free(sequence.seq); sequence.seq = NULL; return -1;
        }

        if (vectorNorm > 1) { // check on a way (queen)
            for (int i = 1; i < vectorNorm; i++)
            {
                struct CheckersCoords displacementVector;
                displacementVector.ia = currentCoords.ia + (movementVector.ia / vectorNorm) * i;
                displacementVector.in = currentCoords.in + (movementVector.in / vectorNorm) * i;
                if (game->board[displacementVector.in][displacementVector.ia] != ' ') {
                    free(sequence.seq); sequence.seq = NULL; return -1;
                }
            }
        }

        game->board[nextCoords.in][nextCoords.ia] = game->board[currentCoords.in][currentCoords.ia];
        game->board[currentCoords.in][currentCoords.ia] = ' ';
        moved = 1;
    
    } else { // strike
        struct CheckersCoords currentCoords = sequence.seq[0];
        for (int i = 1; i < sequence.seqLen; i++) {
            struct CheckersCoords nextCoords = sequence.seq[i];
            if (game->board[nextCoords.in][nextCoords.ia] != ' ' || game->board[(currentCoords.in + nextCoords.in)/2][(currentCoords.ia + nextCoords.ia)/2] != opponent) {
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
                striked = 1;
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