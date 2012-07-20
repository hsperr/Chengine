//
//  Chengine.h
//  Chengine
//
//  Created by Henning Sperr on 6/26/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Chengine_Chengine_h
#define Chengine_Chengine_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
#include <math.h>

#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif


typedef unsigned char PIECE;
typedef enum Color {WHITE=0,BLACK=1}Color;


typedef enum {
    ChError_OK = 0,
    ChError_Arguments,
    ChError_Resources,
    ChError_BrokenFenString,
    ChError_RepetitionDraw,
    ChError_NotInTable,
    ChError_DepthToLow,
    ChError_IllegalMove,
    ChError_CheckMate,
    ChError_StaleMate
} ChError;

typedef struct PieceScores{
    char pieceCounts[6]; //pnkbrq
    int totalScores;
}PieceScores;

typedef struct PieceInfo{
    PIECE piece;
    int location;
    Color color;
    int score;
}PieceInfo;

enum MoveType{
    NORMAL,
    PAWNDOUBLE,
    PROMOTION,
    ENPASSANT,
    WKINGCASTLE,
    WQUEENCASTLE,
    BKINGCASTLE,
    BQUEENCASTLE
}MoveType;

typedef struct History{
    //capture
    int previousEnPassantSquare;
    PieceInfo* capturedPiece;//also used for enpassant
    int oldRepetitionMoves;
    int castlingRights;
    u_int64_t zobrist;
    PieceScores oldScores[2];
}History;

typedef struct Move{
    unsigned char from;
    unsigned char to;
    unsigned char promote;
    unsigned char moveType;
}Move;

typedef struct MoveList{
    int nextFree;
    int alloc;
    Move* array;
}MoveList;
typedef struct Properties{
    int depth;
    int timelimit;
    int isAi;
    char useOpeningTable;
}Properties;

enum {
    pawn,
    knight,
    king,
    bishop,
    rook,
    queen
};

typedef enum GamePhase{
Opening,
MiddleGame,
EndGame
}GamePhase;


typedef struct ChessBoard{
    //size of fixed 16x8 bit
    //00 01 02 03 04 05 06 07   08 09 0A 0B 0C 0D 0E 0F
    //10 11 12 13 14 15 16 17   18 19 1A 1B 1C 1D 1E 1F
    //20 21 22 23 24 25 26 27   28 29 2A 2B 2C 2D 2E 2F
    //30 31 32 33 34 35 36 37   38 39 3A 3B 3C 3D 3E 3F
    //40 41 42 43 44 45 46 47   48 49 4A 4B 4C 4D 4E 4F
    //50 51 52 53 54 55 56 57   58 59 5A 5B 5C 5D 5E 5F
    //60 61 62 63 64 65 66 67   68 69 6A 6B 6C 6D 6E 6F
    //70 71 72 73 74 75 76 77   78 79 7A 7B 7C 7D 7E 7F
    
    // Move is legal if sqare&0x88==0
    PieceInfo* tiles[128]; 
    //pieceToSquare is rnbqkbnrpppppppp
    //                 RNBWKBNRPPPPPPPP
    //gets updated in doMove() -1 denotes piece is off the board
    PieceInfo blackToSquare[16];
    PieceInfo whiteToSquare[16];
    PieceScores playerScores[2];
    GamePhase phase;
    
    Color colorToPlay;
    MoveList playedMoves;
    int enPassantSquare;
    int repetitionMoves;
    int castlingRights;
    char hasCastled; //0 nobody 1 white 2 black 3 both
    u_int64_t zobrist;
}ChessBoard;

typedef struct Game{
    ChessBoard board;
    int isRunning;
    Properties Player[2];
}Game;

ChError addToMoveList(MoveList* moveList, Move* move);
void freeMoveList(MoveList* moveList);
void printMoveList(MoveList* moveList);
void printMoveListFromOffset(MoveList* moveList, int fromOffset);
void printError(ChError hr);

#include "Board.h"
#include "perft.h"
#include "NegaScout.h"
#include "Input.h"
#include "TranspositionTable.h"

#endif
