#ifndef CHECKERSINPUTPARSING_H
#define CHECKERSINPUTPARSING_H

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

int checkersCoordsInBounds(struct CheckersCoords* coords);
int allowedCharInCharPos(char* character);
struct CheckersCoords charPosToCoords(char* charPos, int charPosLen);
struct CheckersCoordsSequence commandToCoordsSequence(char* command, int commandCharLen);

#endif