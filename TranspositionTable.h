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

ChError initTable(long size);
u_int64_t getZobristHash(ChessBoard* board);

ChError addKeyToTable(u_int64_t zobrist, int depth, int score, int bound,Move move);
ChError probe(u_int64_t zobrist, int depth, int* alpha, int* beta, int* score, Move* move);

ChError addPieceZobrist(u_int64_t* zobrist,int location,PIECE piece, Color color);
ChError removePieceZobrist(u_int64_t* zobrist,int location,PIECE piece, Color color);
ChError switchColorZobrist(u_int64_t* zobrist);
ChError setEnPassantZobrist(u_int64_t* zobrist, int oldEnPassant, int newEnPassant);
ChError updateCastleRightZobrist(u_int64_t* zobrist, int nr); //KQkq
ChError clearTable(void);

ChError incrementRepetitionTable(u_int64_t* zobrist);
ChError decrementRepetitionTable(u_int64_t* zobrist);

long probeRepetitionTable(u_int64_t* zobrist);

void freeTable(void);


#endif
