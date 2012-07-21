//
//  TranspositionTable.h
//  Chengine
//
//  Created by Henning Sperr on 7/2/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Chengine_TranspositionTable_h
#define Chengine_TranspositionTable_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "Chengine.h"


typedef enum Bound{
    HASH_EXACT,
    HASH_ALPHA,
    HASH_BETA
}Bound;

ChError initHashTable(long size);
ChError initRepetitionTable(long size);
u_int64_t getZobristHash(ChessBoard* board);

ChError addKeyToTable(u_int64_t zobrist, int depth, int score, Bound bound,Move move);
ChError probe(u_int64_t zobrist, int depth, int* alpha, int* beta, int* score, Move* move);

ChError addPieceZobrist(u_int64_t* zobrist,int location,PIECE piece, Color color);
ChError removePieceZobrist(u_int64_t* zobrist,int location,PIECE piece, Color color);
ChError switchColorZobrist(u_int64_t* zobrist);
ChError setEnPassantZobrist(u_int64_t* zobrist, int oldEnPassant, int newEnPassant);
ChError updateCastleRightZobrist(u_int64_t* zobrist, int nr); //KQkq

ChError clearHashTable(void);
void clearRepetitionTable(void);

ChError incrementRepetitionTable(u_int64_t* zobrist);
ChError decrementRepetitionTable(u_int64_t* zobrist);

int probeRepetitionTable(u_int64_t* zobrist);

void freeTable(void);


ChError initEvalTable(long size);
ChError clearEvalTable();
ChError probeEvalTable(u_int64_t* zobrist, int* eval);
ChError addToEvalTable(u_int64_t* zobrist, int eval);


#endif
