//
//  Eval.h
//  Chengine
//
//  Created by Henning Sperr on 7/17/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Chengine_Eval_h
#define Chengine_Eval_h

#include "Chengine.h"
#include "Board.h"
#include "TranspositionTable.h"

int EvaluateComplex(ChessBoard* board);
int evaluate(ChessBoard* board);
int getExpectedMoveScoreChange(ChessBoard* board, Move* move);
#endif
