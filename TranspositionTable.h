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

ChError initTable(long size, long inMask);
u_int64_t getZobristHash(ChessBoard* board);

ChError addKeyToTable(u_int64_t zobrist, int depth, int score);
ChError probe(u_int64_t zobrist, int depth,int* score);

ChError addPieceZobrist(u_int64_t* zobrist,int location,PIECE piece, Color color);
ChError removePieceZobrist(u_int64_t* zobrist,int location,PIECE piece, Color color);
ChError switchColorZobrist(u_int64_t* zobrist);
ChError setEnPassantZobrist(u_int64_t* zobrist, int oldEnPassant, int newEnPassant);
ChError updateCastleRightZobrist(u_int64_t* zobrist, int nr); //KQkq

void freeTable(void);


#endif
