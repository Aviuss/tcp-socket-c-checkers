#include "checkersInputParsing.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int checkersCoordsInBounds(struct CheckersCoords* coords) {
    if (coords->ia < 0 || coords->ia > 7 || coords->in < 0 || coords->in > 7) {
        return -1;
    }
    return 1;
}

int allowedCharInCharPos(char* character) {
    if ((*character)-'A' >= 0 && (*character)-'A'<8) {
        return 1;
    }
    if ((*character)-'1' >= 0 && (*character)-'1'<8) {
        return 1;
    }
    if ((*character) == '-' || (*character) == '>') {
        return 1;
    }
    return -1;
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
    while(commandCharLen > 0 && allowedCharInCharPos(command) == -1) {
        command++;
        commandCharLen--;
    }

    char *end = command + commandCharLen - 1;
    while(commandCharLen > 0 && end > command && allowedCharInCharPos(end) == -1) {
        end--;
        commandCharLen--;
    } 

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